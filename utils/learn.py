import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt

data=np.load('data.npz')
temp=np.load('tem.npz')
vald=np.load('vald.npz')
valt=np.load('valttem.npz')
data=data['arr_0']
temp=temp['arr_0']
vald=vald['arr_0']
valt=valt['arr_0']
TCl=np.linspace(2,3,20)
accur=[]
for Tc in TCl:
    par=np.array(list(map(lambda x: 0 if x<Tc else 1,temp)))
    vpar=np.array(list(map(lambda x: 0 if x<Tc else 1,valt)))
    model = tf.keras.Sequential([
    tf.keras.layers.Dense(100, activation='relu'),
    tf.keras.layers.Dense(10),
    tf.keras.layers.Dense(2,activation='softmax')
    ])
    model.compile(optimizer='adam',
              loss=tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True),
              metrics=['accuracy'])
    model.fit(data, par, epochs=5)
    accur.append(model.evaluate(vald,vpar)[1])
plt.plot(TCl,accur)
plt.grid()
plt.savefig("accuracy.png")
plt.show()