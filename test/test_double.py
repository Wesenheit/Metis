import ParaIsing
import numpy as np
from matplotlib import pyplot as plt
import time
start=time.time()
n=100
k=100
T=np.linspace(0.0001,4,50)
m=[]
p=ParaIsing.Board(n)
print("burnout")
p.evolvep(10**6,0.001,0,4,2)
print("evolution")
for t in T:
    print(t)
    temp=np.zeros(k)
    p.evolvep(10**6,t,0,4,2)
    for i in range(k):
        p.evolvep(10**6,t,0,4,2)
        temp[i]=(abs(p.mean()))
    m.append(np.mean(temp))
end=time.time()
print("time:",end-start)
plt.plot(T,m)
plt.show()

