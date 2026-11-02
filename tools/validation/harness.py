from pathlib import Path
import subprocess,selectors,os,time,json,re,signal,tempfile,shutil,hashlib,socket
ROOT=Path(__file__).resolve().parents[2]
OUT=Path(os.environ.get('XV6_RESULTS', str(ROOT/'validation-output'))).resolve();OUT.mkdir(parents=True,exist_ok=True)
HUB_AVAILABLE='hub' in subprocess.check_output(['qemu-system-riscv64','-chardev','help'],text=True)
class Guest:
 def __init__(self,name,source=None,cpus=1,ram=128,policy=1,defines=None,template=None):
  self.out=OUT/name;self.out.mkdir(exist_ok=False);self.name=name;self.commands=[]
  self.runtime=Path(tempfile.mkdtemp(prefix='xv6-validate-'))
  self.config=dict(cpus=cpus,ram=ram,policy=policy,defines=defines or {})
  if template is None:
   source=Path(source or ROOT)
   (OUT/'status.json').write_text(json.dumps(dict(state='copying',run=name)))
   manifest={}
   for directory in ['kernel','user','mkfs']:
    (self.runtime/directory).mkdir()
    for f in (source/directory).iterdir():
     if f.suffix in ['.c','.h','.S','.ld','.pl'] and f.name != 'usys.S':
      shutil.copyfile(f,self.runtime/directory/f.name);manifest[directory+'/'+f.name]=hashlib.sha256(f.read_bytes()).hexdigest()
   for filename in ['Makefile','README','LICENSE']:
    shutil.copyfile(source/filename,self.runtime/filename);manifest[filename]=hashlib.sha256((source/filename).read_bytes()).hexdigest()
   self.meta={'name':name,'configuration':self.config,'source_files':manifest,'source_digest':hashlib.sha256(json.dumps(manifest,sort_keys=True).encode()).hexdigest(),'runtime_filesystem':'WSL native temporary filesystem'}
   (self.out/'source.json').write_text(json.dumps(self.meta,indent=2));self.status('building')
   args=['make','-j4','kernel/kernel','fs.img',f'CPUS={cpus}',f'RAM_MB={ram}',f'SWAP_POLICY={policy}']
   args += [f'{k}={v}' for k,v in (defines or {}).items()]
   with (self.out/'build.log').open('w') as f:subprocess.run(args,cwd=self.runtime,stdout=f,stderr=subprocess.STDOUT,check=True)
  else:
   assert self.config == template.config
   self.meta={k:v for k,v in template.meta.items() if k not in ['commands','console_sha256']}
   self.meta['name']=name
   self.meta['build_reused_from']=template.name
   (self.runtime/'kernel').mkdir()
   shutil.copyfile(template.runtime/'kernel/kernel',self.runtime/'kernel/kernel')
   shutil.copyfile(template.pristine,self.runtime/'fs.img')
   (self.out/'source.json').write_text(json.dumps(self.meta,indent=2))
  self.pristine=self.runtime/'pristine.img'
  shutil.copyfile(self.runtime/'fs.img',self.pristine)
  self.meta['kernel_sha256']=hashlib.sha256((self.runtime/'kernel/kernel').read_bytes()).hexdigest()
  self.temporary_instance=template is not None
  self.monitor=self.runtime/'monitor.sock'
  args=['qemu-system-riscv64','-machine','virt','-bios','none','-kernel','kernel/kernel','-m','128M','-smp',str(cpus),'-display','none','-monitor',f'unix:{self.monitor},server=on,wait=off','-chardev','stdio,id=stdio0,signal=off','-chardev','vc,id=vc0,cols=90,rows=30','-chardev','hub,id=hub0,chardevs.0=stdio0,chardevs.1=vc0','-serial','chardev:hub0','-global','virtio-mmio.force-legacy=false','-drive','file=fs.img,if=none,format=raw,id=x0','-device','virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0']
  if not HUB_AVAILABLE:
   start=args.index('-chardev');end=args.index('-serial')+2
   args=args[:start]+['-chardev','stdio,id=stdio0,signal=off','-serial','chardev:stdio0']+args[end:]
  self.p=subprocess.Popen(args,cwd=self.runtime,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,start_new_session=True,bufsize=0)
  self.sel=selectors.DefaultSelector();self.sel.register(self.p.stdout,selectors.EVENT_READ);self.log=(self.out/'console.log').open('wb')
  print(self.readuntil(timeout=60),flush=True);self.status('ready')
 def status(self,state,**kwargs):
  text=json.dumps(dict(state=state,run=self.name,config=self.config,**kwargs),indent=2)
  (OUT/'status.json').write_text(text);(self.out/'status.json').write_text(text)
 def readuntil(self,timeout=600,prompt=r'(?:^|\r?\n)\$ $'):
  data=bytearray();end=time.monotonic()+timeout
  while time.monotonic()<end:
   for k,_ in self.sel.select(.2):
    c=os.read(k.fileobj.fileno(),65536)
    if not c:raise RuntimeError('QEMU ended: '+data.decode(errors='replace')[-2000:])
    self.log.write(c);self.log.flush();data.extend(c)
    s=data.decode(errors='replace')
    if 'panic:' in s:raise RuntimeError(s[-2000:])
    if re.search(prompt,s):return s
  raise TimeoutError(data.decode(errors='replace')[-2000:])
 def cmd(self,command,marker=None,timeout=600,allow_failure=False):
  self.status('command',command=command);start=time.monotonic()
  self.p.stdin.write((command+'\n').encode());self.p.stdin.flush();out=self.readuntil(timeout)
  filename=f'{len(self.commands)+1:02d}-'+re.sub('[^A-Za-z0-9]','-',command)+'.txt'
  (self.out/filename).write_text(out);entry=dict(command=command,file=filename,host_seconds=time.monotonic()-start);self.commands.append(entry)
  print(out,flush=True)
  if marker and marker not in out:raise RuntimeError('Missing '+marker+' for '+command)
  if not allow_failure and re.search(r'FAILED|FAIL\b|exec .* failed',out):raise RuntimeError('Failed '+command)
  return out
 def capture(self,label):
  if not HUB_AVAILABLE:return False
  sock=socket.socket(socket.AF_UNIX);sock.connect(str(self.monitor));sock.settimeout(3)
  try:
   sock.recv(4096);path=self.runtime/(label+'.ppm');sock.sendall(('screendump '+str(path)+'\n').encode());reply=sock.recv(8192).decode(errors='replace')
   time.sleep(.3)
   if path.exists():
    shutil.copyfile(path,self.out/(label+'.ppm'));return True
   (self.out/(label+'-capture-error.txt')).write_text(reply);return False
  finally:sock.close()
 def close(self):
  self.p.terminate()
  try:self.p.wait(timeout=8)
  except subprocess.TimeoutExpired:self.p.kill();self.p.wait()
  self.log.close();self.meta['commands']=self.commands;self.meta['console_sha256']=hashlib.sha256((self.out/'console.log').read_bytes()).hexdigest()
  (self.out/'run.json').write_text(json.dumps(self.meta,indent=2))
  if self.temporary_instance:self.cleanup()

 def cleanup(self):
  resolved=self.runtime.resolve()
  assert resolved.parent == Path('/tmp') and resolved.name.startswith('xv6-validate-')
  shutil.rmtree(resolved)
