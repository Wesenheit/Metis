import Ising
import numpy as np
from matplotlib import pyplot as plt
import time
start=time.time()
n=1500
k=100
T=np.linspace(0.0001,4,50)
m=[]
p=Ising.Board(n,1)
print("burnout")
p.evolve_per(10**11,0.001,0)
print("evolution")
for t in T:
    print(t)
    temp=np.zeros(k)
    p.evolve_per(10**8,t,0)
    for i in range(k):
        p.evolve_per(10**7,t,0)
        temp[i]=(abs(p.mean()))
    m.append(np.mean(temp))
end=time.time()
print("time:",end-start)
plt.plot(T,m)
plt.show()

