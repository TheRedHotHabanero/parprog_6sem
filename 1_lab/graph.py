import numpy as np
from matplotlib import pyplot as plt
from pathlib import Path
from mpl_toolkits.mplot3d import Axes3D

file = Path("result.dat")

params = {
"tau": 0,
"h": 0,
"T": 0,
"X": 0,
"M": 0,
"K": 0
}
vals = []

with open(file) as f:
    for line in f:
        line = line.split()
        name = line[0]
        if name[:-1] in params.keys():
            params[name[:-1]] = float(line[1])
        else:
            vals.append(float(name))
      
U = np.zeros((int(params['K']), int(params['M'])))
for i in range(U.shape[0]):
    for j in range(U.shape[1]):
        U[i][j] = vals[i * U.shape[1] + j]

X = np.array([k * params['h'] for k in range(U.shape[1])])
T = np.array([k * params['tau'] for k in range(U.shape[0])])

X, T = np.meshgrid(X, T)
fg = plt.figure(figsize=(22, 15))
ax = Axes3D(fg)
ax = fg.add_subplot(111, projection='3d')
ax.plot_surface(X, T, U, cmap='plasma')
ax.set_xlabel("$x$", fontsize=25)
ax.set_ylabel("$t$", fontsize=25)
ax.set_zlabel("$u(x, t)$", fontsize=25)
plt.savefig("result.png")
plt.show()