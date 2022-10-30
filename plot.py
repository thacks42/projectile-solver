from mpl_toolkits import mplot3d
import numpy as np
import matplotlib.pyplot as plt
import sys


fig = plt.figure()
ax = plt.axes(projection='3d')

colors = ["red", "green", "blue", "yellow"]
c_index = 0

for n,file_name in enumerate(sys.argv[1:]):
    x_data = []
    y_data = []
    z_data = []
    

    
    with open(file_name, 'r') as f:
        x1, y1, z1, x2, y2, z2= f.readline().split()
        x = [float(x1), float(x2)]
        y = [float(y1), float(y2)]
        z = [float(z1), float(z2)]
        ax.scatter(x, y, z, s=40.0, marker='o')
        for line in f:
            t,x,y,z = line.split()
            x_data.append(float(x))
            y_data.append(float(y))
            z_data.append(float(z))

    ax.plot3D(x_data, y_data, z_data, colors[c_index], label = "solution #" + str(n))
    c_index = c_index + 1
    if c_index == len(colors):
        c_index = 0

ax.legend()
plt.show();
