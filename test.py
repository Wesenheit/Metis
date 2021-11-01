from matplotlib import pyplot as plt
import ising
import numpy as np
T=10
p=ising.Board(7,T)
B=np.linspace(0,1000,100)
y=[]
for b in B:
	p.MC(2000,T,b)
	y.append(p.mean())
plt.plot(B,y)
plt.savefig("test.png")


