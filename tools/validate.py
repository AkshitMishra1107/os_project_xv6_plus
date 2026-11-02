#!/usr/bin/env python3
"""Reproduce xv6 checks, causal benchmarks and replacement reference models."""
import argparse,csv,hashlib,json,os,re,statistics,struct,subprocess,tempfile
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--suite',choices=['smoke','full','bench','models','all'],default='smoke')
parser.add_argument('--out',type=Path,default=Path('validation-output'))
parser.add_argument('--reps',type=int,default=5)
parser.add_argument('--refs',type=int,default=6000)
args=parser.parse_args()
if os.name!='posix':parser.error('Run this command in Ubuntu or WSL.')
if args.reps<1 or args.refs<100:parser.error('Positive repetitions and at least 100 references are required.')
os.environ['XV6_RESULTS']=str(args.out.resolve())
from validation.harness import Guest,OUT,ROOT
from validation.models import trace,checksum,faults,working_set,selftest

def environment():
    versions={}
    for name,cmd in [('gcc',['riscv64-linux-gnu-gcc','--version']),('qemu',['qemu-system-riscv64','--version']),('kernel',['uname','-r'])]:
        versions[name]=subprocess.check_output(cmd,text=True).splitlines()[0]
    versions['source_revision']=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip()
    versions['source_clean']=subprocess.run(['git','diff','--quiet','HEAD','--','kernel','user','Makefile'],cwd=ROOT).returncode==0
    # Verify the virtual platform frequency used by uptime_us.
    dtb=OUT/'virt.dtb'
    subprocess.run(['qemu-system-riscv64','-machine','virt,dumpdtb='+str(dtb),'-bios','none','-display','none'],capture_output=True,check=True,timeout=20)
    data=dtb.read_bytes();strings=struct.unpack_from('>I',data,12)[0]
    off=data[strings:].index(b'timebase-frequency\0')
    prop=struct.pack('>III',3,4,off);pos=data.index(prop)
    versions['timebase_hz']=struct.unpack_from('>I',data,pos+12)[0]
    assert versions['timebase_hz']==10000000,versions
    with tempfile.TemporaryDirectory(prefix='xv6-pattern-') as temp:
        binary=Path(temp)/'pattern-check'
        subprocess.run(['cc','-Wall','-Wextra','-Werror','-O2',str(ROOT/'tools/validation/pattern_check.c'),'-o',str(binary)],check=True)
        test=subprocess.check_output([str(binary)],text=True)
        (OUT/'pattern-check.txt').write_text(test)
    (OUT/'environment.json').write_text(json.dumps(versions,indent=2))
    return versions

def run_full(case):
    cpus,policy=case;g=None
    try:
        g=Guest(f'full-cpu{cpus}-p{policy}',cpus=cpus,ram=128,policy=policy)
        g.cmd('memstat','free pages');g.cmd('usertests','ALL TESTS PASSED',timeout=7200)
        g.cmd('memstat','free pages')
        if cpus==1:g.cmd('swapcheck','swapcheck: OK')
        g.capture('full-pass');g.status('complete')
        return {'cpus':cpus,'policy':policy,'passed':True}
    finally:
        if g:g.close();g.cleanup()

def smoke():
    g=None
    try:
        g=Guest('smoke-smp',cpus=3)
        for c,m in [('baseline','succeeded'),('lazytest','lazytest: OK'),('cowtest','cowtest: OK'),('kthreadtest','kthreadtest: OK'),('threadlife','threadlife: OK'),('threadmem','threadmem: OK'),('futextest','futextest: OK'),('libtest','libtest: OK'),('threadtest','499500'),('exittest','exittest: OK'),('racetest','racetest: OK'),('prodcons','prodcons: OK'),('philosophers','philosophers: OK'),('semtest','semtest: OK'),('sigtest','sigtest: ALL OK'),('tracethread','tracethread: OK'),('trace 128 echo hi','syscall exec -> 2'),('dbgmini','final reason 3'),('dbglife','dbglife: OK'),('finaldemo','full-page bad=0')]:
            g.cmd(c,m)
            if c in ['cowtest','racetest','sigtest','dbgmini']:g.capture(c+'-result')
        g.capture('feature-checks');g.close();g.cleanup();g=None
        for policy in [1,2,3]:
            g=Guest('smoke-p'+str(policy),cpus=1,ram=8,policy=policy)
            g.cmd('swapcheck','swapcheck: OK')
            for c,m in [('memhog 4','child exit status 0'),('memhog 12','child exit status 0'),('swapfork','swapfork: OK'),('threadswap','threadswap: OK')]:
                g.cmd(c,m)
            g.cmd('swapcheck','swapcheck: OK');g.capture('integrity-owners')
            g.cmd('finaldemo 4','full-page bad=0');g.cmd('swapcheck','swapcheck: OK');g.capture('combined-paging')
            g.close();g.cleanup();g=None
    finally:
        if g:g.close();g.cleanup()

def models():
    result={'belady_example':selftest(),'frame_budget':1700,'initialization':list(range(3072)),'cases':[]}
    (OUT/'traces').mkdir(exist_ok=True)
    for kind in ['seq','uniform','hot','phase']:
        for seed in range(1,args.reps+1):
            refs=trace(kind,seed,args.refs);raw='\n'.join(map(str,refs))+'\n'
            path=OUT/'traces'/f'{kind}-{seed}.txt';path.write_text(raw)
            result['cases'].append({'pattern':kind,'seed':seed,'refs':len(refs),'sha256':hashlib.sha256(raw.encode()).hexdigest(),
                'checksum':checksum(refs,seed),'working_set_128_mean':working_set(refs),
                'faults':{p:faults(list(range(3072))+refs,1700,p,warmup=3072) for p in ['FIFO','Clock','LRU','OPT']}})
    (OUT/'reference-models.json').write_text(json.dumps(result,indent=2))

def benchmark():
    cache={};rows=[]
    def build(cpus,ram,policy,cow=1):
        key=f'cpu{cpus}-ram{ram}-p{policy}-cow{cow}'
        if key not in cache:
            g=Guest('build-'+key,cpus=cpus,ram=ram,policy=policy,defines={'FORK_COW':cow})
            g.close();cache[key]=g
        return cache[key]
    def guest(name,template):
        cfg=template.config
        return Guest(name,cpus=cfg['cpus'],ram=cfg['ram'],policy=cfg['policy'],defines=cfg['defines'],template=template)
    def record(row):
        rows.append(row);(OUT/'samples.json').write_text(json.dumps(rows,indent=2))
        print('Recorded',row,flush=True)
    try:
        # Compile first, then rotate policy order for paired inputs.
        for p in [1,2,3]:build(1,8,p)
        for kind in ['seq','uniform','hot','phase']:
            for seed in range(1,args.reps+1):
                order=[1,2,3];order=order[(seed-1)%3:]+order[:(seed-1)%3]
                for p in order:
                    g=guest(f'paging-{kind}-s{seed}-p{p}',build(1,8,p))
                    try:
                        s=g.cmd(f'pagebench {kind} {args.refs} {seed}','pagebench: OK',timeout=1200)
                        elapsed=int(re.search(r'elapsed_us=(\d+)',s)[1]);m=re.search(r'faults=(\d+) swap_in=(\d+) swap_out=(\d+) checksum=(\d+) bad=(\d+)',s)
                        values=list(map(int,m.groups()));assert values[3]==checksum(trace(kind,seed,args.refs),seed) and values[4]==0
                        record(dict(kind='paging',pattern=kind,seed=seed,policy=p,elapsed_us=elapsed,faults=values[0],swap_in=values[1],swap_out=values[2],checksum=values[3],bad=values[4]))
                        if seed==1:g.capture('paging-result')
                    finally:g.close()
        for cow in [0,1]:build(1,128,1,cow)
        for mode in ['readonly','writeall']:
            for rep in range(1,args.reps+1):
                for cow in ([0,1] if rep%2 else [1,0]):
                    g=guest(f'fork-{mode}-r{rep}-cow{cow}',build(1,128,1,cow))
                    try:
                        s=g.cmd('forkbench '+mode+' 10','forkbench: OK')
                        m=re.search(r'elapsed_us=(\d+) cow_copies=(\d+)',s)
                        record(dict(kind='fork',mode=mode,rep=rep,cow=cow,elapsed_us=int(m[1]),cow_copies=int(m[2]),rounds=10))
                        if rep==1:g.capture('fork-result')
                    finally:g.close()
        build(3,128,1)
        for rep in range(1,args.reps+1):
            order=[1,2,3];order=order[(rep-1)%3:]+order[:(rep-1)%3]
            for workers in order:
                g=guest(f'scaling-r{rep}-w{workers}',build(3,128,1))
                try:
                    s=g.cmd(f'speedup {workers} 400000000','speedup: OK')
                    m=re.search(r'elapsed_us=(\d+) checksum=(\d+)',s);assert int(m[2])==1914453504
                    record(dict(kind='scaling',rep=rep,workers=workers,cpus=3,elapsed_us=int(m[1]),checksum=int(m[2])))
                    if rep==1:g.capture('scaling-result')
                finally:g.close()
    finally:
        for g in cache.values():g.cleanup()
    fields=sorted({key for r in rows for key in r})
    with (OUT/'samples.csv').open('w',newline='') as f:
        writer=csv.DictWriter(f,fieldnames=fields);writer.writeheader();writer.writerows(rows)

environment()
if args.suite in ['models','bench','all']:models()
if args.suite in ['smoke','all']:smoke()
if args.suite in ['full','all']:
    with ThreadPoolExecutor(max_workers=3) as pool:
        results=list(pool.map(run_full,[(1,1),(1,2),(1,3),(3,1)]))
    (OUT/'full-results.json').write_text(json.dumps(results,indent=2))
if args.suite in ['bench','all']:benchmark()
(OUT/'complete.json').write_text(json.dumps({'suite':args.suite,'complete':True},indent=2))
print('Validation complete:',args.suite,flush=True)
