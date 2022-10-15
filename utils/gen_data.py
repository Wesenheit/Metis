import numpy as np
from Ising import Board
import multiprocessing as mp
import time
from sys import argv
def to_array(st):
    wyn=[]
    for l in st:
        if l=="1":
            wyn.append(1)
        else:
            wyn.append(-1)
    return wyn
n=10
k=10
u=10
num_t=10
T=np.linspace(0.001,4,num_t)
tem=[]
data=[]
cores=10
t1=time.time()
for t in T:
    print(t)
    for _ in range(k):
        p=Board(n)
        p.evolve_per(10**7,t,0)
        for _ in range(u):
            data.append(to_array(p.flatten()))
            tem.append(t)
            p.evolve_per(100,t,0)
datam=[]
tempm=[]
for i in range(len(data)):
    new=[]
    for l in data[i]:
        new.append(-l)
    datam.append(new)
    tempm.append(tem[i])
data=[*data,*datam]
tem=[*tem,*tempm]
data=np.array(data,dtype=np.int8)
tem=np.array(tem,dtype=np.float)
np.savez_compressed(argv[1]+".npz",data)
np.savez_compressed(argv[2]+"tem.npz",tem)
t2=time.time()
print("produced {} data, it took {}".format(k*u*num_t,t2-t1))


