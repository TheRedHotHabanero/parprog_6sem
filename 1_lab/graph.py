import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from mpl_toolkits.mplot3d import Axes3D

data = pd.read_csv("result.csv", sep=" ")
data.sort_values(by='x', inplace=True)

# Извлечение координат и значений функции u
x = np.array(data.iloc[:-1, 0].tolist())
t = np.array(data.iloc[:-1, 1].tolist())
u = np.array(data.iloc[:-1, 2].tolist())

# Извлечение размерности сетки
N = int(len(x) - 1)
M = int(len(t) / (N+1))
assert len(t) == M * (N+1)

def plot():
    # Построение трехмерного графика
    fig = plt.figure(figsize=[16,18])
    ax = fig.add_subplot(111, projection ='3d')

    im = ax.scatter(x, t, u, cmap='coolwarm', c = u)
    #im = ax.plot_surface(x, t, U, cmap='coolwarm', c = u)
    cbar = fig.colorbar(im, orientation='vertical')

    ax.set_xlabel("x")
    ax.set_ylabel("t")
    ax.set_zlabel("u")

    ax.set_xlim([0, 1])
    ax.set_ylim([0, 1])
    ax.set_zlim([-1,1.5])

    plt.title("График последовательного решения")
    plt.show()

def main():
    plot()

if __name__ == '__main__':
    main()
