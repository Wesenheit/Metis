import Ising
import numpy as np
from matplotlib import pyplot as plt
n=15
k=30
T=np.linspace(0.0001,4,100)
m=[]
def mean(lista):
    wyn=0
    for a in lista:
        wyn+=a/len(lista)
    return wyn

for t in T:
    print(t)
    temp=[]
    for _ in range(k):
        p=Ising.Board(n)
        p.evolve_per(10**6,t,0)
        temp.append(abs(p.mean()))
    m.append(mean(temp))
plt.plot(T,m)
plt.show()

