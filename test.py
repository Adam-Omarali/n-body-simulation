import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# G = 6.67e-11
G = 1
dt = 0.01
steps = 25000

m1 = 1
m2 = 1
p_initial_m1 = np.array([1, 0])
p_initial_m2 = np.array([-1, 0])
v_initial_m1 = np.array([0, 1])
v_initial_m2 = np.array([0, -1])

p_m1 = [p_initial_m1]
p_m2 = [p_initial_m2]
f_m1 = [[0, 0]]
f_m2 = [[0, 0]]
v_m1 = [v_initial_m1]
v_m2 = [v_initial_m2]

def magnitude(r: np.array) -> float:
    squared = 0
    for _, e in enumerate(r):
        squared += e*e
    
    return np.sqrt(squared)

def gravitational_force(m1: float, m2: float, r: np.array) -> np.array:
    return G * m1 * m2 * r / (magnitude(r)**3)

def update_velocity(v_prev: np.array, force: np.array, m: float) -> np.array:
    return v_prev + force / m * dt

def update_position(p_prev: np.array, v: np.array) -> np.array:
    return p_prev + v * dt


def simulate(steps):
    for i in range(1, steps):
        r = p_m1[i - 1] - p_m2[i - 1]
        f12 = gravitational_force(m1, m2, r)
        f21 = -f12
        f_m1.append(f21)
        f_m2.append(f12)
        v_m1.append(update_velocity(v_m1[i - 1], f21, m1))
        v_m2.append(update_velocity(v_m2[i - 1], f12, m2))
        p_m1.append(update_position(p_m1[i - 1], v_m1[i]))
        p_m2.append(update_position(p_m2[i - 1], v_m2[i]))
        
simulate(steps)

plt.plot(list(map(lambda p: p[0], p_m1)), list(map(lambda p: p[1], p_m1)), 'ro')
plt.plot(list(map(lambda p: p[0], p_m2)), list(map(lambda p: p[1], p_m2)), 'bo')
plt.xlabel('x')
plt.ylabel('y')
plt.show()
    