"""Deterministic reference traces and fully specified replacement models."""
from collections import deque, OrderedDict, defaultdict
import heapq

def rng(state):
    state ^= (state << 13) & 0xffffffff
    state ^= state >> 17
    state ^= (state << 5) & 0xffffffff
    return state & 0xffffffff

def trace(kind, seed=1, refs=6000, pages=3072):
    state = seed
    result = []
    for i in range(refs):
        state = rng(state)
        if kind == 'seq':
            page = i % pages
        elif kind == 'uniform':
            page = state % pages
        else:
            phase = i * 3 // refs if kind == 'phase' else 0
            page = phase * 1024 + ((state >> 8) % 128) if state % 100 < 90 else (state >> 8) % pages
        result.append(page)
    return result

def checksum(refs, seed):
    total = 0
    for page in refs:
        x = (((page + 1) * 2654435761) ^ 2246822519 ^ seed) & 0xffffffff
        x ^= x >> 16
        total = (total + x * 3266489917) & 0xffffffff
    return total

def faults(sequence, frames, policy, warmup=0):
    resident = set()
    order = deque()
    lru = OrderedDict()
    bits = {}
    hand = 0
    clock = []
    future = defaultdict(deque)
    heap = []
    next_use = {}
    if policy == 'OPT':
        for i, p in enumerate(sequence):
            future[p].append(i)
    count = 0
    for i, page in enumerate(sequence):
        if policy == 'OPT':
            future[page].popleft()
            next_use[page] = future[page][0] if future[page] else len(sequence) + 1
        hit = page in resident
        if not hit:
            if i >= warmup:
                count += 1
            if len(resident) == frames:
                if policy == 'FIFO':
                    victim = order.popleft()
                elif policy == 'LRU':
                    victim, _ = lru.popitem(last=False)
                elif policy == 'Clock':
                    while bits[clock[hand]]:
                        bits[clock[hand]] = 0
                        hand = (hand + 1) % frames
                    victim = clock[hand]
                    clock[hand] = page
                    hand = (hand + 1) % frames
                elif policy == 'OPT':
                    while True:
                        priority, victim = heapq.heappop(heap)
                        if victim in resident and -priority == next_use[victim]:
                            break
                else:
                    raise ValueError(policy)
                resident.remove(victim)
            elif policy == 'Clock':
                clock.append(page)
            resident.add(page)
            if policy == 'FIFO':
                order.append(page)
        if policy == 'LRU':
            lru[page] = True
            lru.move_to_end(page)
        if policy == 'Clock':
            bits[page] = 1
        if policy == 'OPT':
            heapq.heappush(heap, (-next_use[page], page))
    return count

def working_set(sequence, window=128):
    counts = defaultdict(int)
    history = deque()
    sizes = []
    for p in sequence:
        counts[p] += 1
        history.append(p)
        if len(history) > window:
            old = history.popleft()
            counts[old] -= 1
            if counts[old] == 0:
                del counts[old]
        if len(history) == window:
            sizes.append(len(counts))
    return sum(sizes) / len(sizes)

def selftest():
    known = [1,2,3,4,1,2,5,1,2,3,4,5]
    assert faults(known, 3, 'FIFO') == 9
    assert faults(known, 4, 'FIFO') == 10
    assert faults(known, 3, 'Clock') == 9
    assert faults(known, 4, 'Clock') == 10
    assert faults(known, 3, 'LRU') == 10
    assert faults(known, 4, 'LRU') == 8
    assert faults(known, 3, 'OPT') == 7
    assert faults(known, 4, 'OPT') == 6
    return {'reference': known, 'FIFO_3': 9, 'FIFO_4': 10, 'LRU_3': 10, 'LRU_4': 8, 'OPT_3': 7, 'OPT_4': 6}
