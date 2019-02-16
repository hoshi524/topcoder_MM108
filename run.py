import subprocess
import json
import threading
import queue
import json
import sys
import os

subprocess.check_call('g++ --std=c++11 -W -Wall -O2 -s -pipe -mmmx -msse -msse2 -msse3 -DLOCAL -o out/main.out {}'.format(sys.argv[1]), shell=True)
subprocess.check_call('javac -d out src/CrosswordPuzzlerVis.java', shell=True)

def solve(seed):
    return float(subprocess.check_output('java -cp out CrosswordPuzzlerVis -exec out/main.out -seed {}'.format(seed), shell=True))

class State:
    count = 0
    rate = 0
    lock = threading.Lock()

    def add(self, seed, score):
        with self.lock:
            self.count = self.count + 1
            self.rate = (self.rate * (self.count - 1) + score) / self.count
            if self.count % 10 == 0:
                print('{}\t{}\t{}'.format(seed, score, self.rate))

state = State()
q = queue.Queue()

def worker():
    while True:
        seed = q.get()
        if seed is None:
            break
        try:
            score = solve(seed)
            state.add(seed, score)
            q.task_done()
        except ValueError as err:
            print("seed {} : {}".format(seed, err))
            os._exit(1)

num_worker_threads = 1
threads = []
for i in range(num_worker_threads):
    t = threading.Thread(target=worker)
    t.start()
    threads.append(t)

for seed in range(1, 100):
    q.put(seed)

q.join()

for i in range(num_worker_threads):
    q.put(None)
for t in threads:
    t.join()

print('{}'.format(state.rate))