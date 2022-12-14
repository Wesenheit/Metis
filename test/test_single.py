import Ising
import numpy as np
from matplotlib import pyplot as plt
import time
def exact(T):
    return np.less(T,2/np.log(1+np.sqrt(2)))*np.power(1-1/np.sinh(2/T)**4,1/8)

start=time.time()
n=1500
k=100
T=np.linspace(0.0001,4,50)
m=[]
p=Ising.Board(n,True)
print("burnout")
p.evolve_heath_bath_fast(10**8,0.001,0)
print("evolution")
for t in T:
    print(t)
    temp=np.zeros(k)
    p.evolve_heath_bath_fast(10**6,t,0)
    for i in range(k):
        p.evolve_heath_bath_fast(10**6,t,0)
        temp[i]=(abs(p.mean()))
    m.append(np.mean(temp))
end=time.time()
print("time:",end-start)
exact_m=exact(T)
plt.plot(T,exact_m)
plt.plot(T,m)
plt.show()

