# Three-Body Simulation — Graphics Quickref

## Option 1: Matplotlib Animation

```python
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# --- Setup ---
fig, ax = plt.subplots()
ax.set_xlim(-L, L)               # set axis bounds
ax.set_ylim(-L, L)
ax.set_aspect('equal')           # critical for orbits

# plot objects — these return lists, grab [0]
body1, = ax.plot([], [], 'o', ms=8)        # single marker
trail1, = ax.plot([], [], '-', lw=0.5)     # trajectory trail

# --- Animation functions ---
def init():
    """Called once. Set empty data."""
    body1.set_data([], [])
    trail1.set_data([], [])
    return body1, trail1

def update(frame):
    """Called each frame. frame = index into your precomputed arrays."""
    body1.set_data([x1[frame]], [y1[frame]])
    trail1.set_data(x1[:frame], y1[:frame])
    return body1, trail1

anim = FuncAnimation(
    fig,
    update,
    frames=num_steps,     # total frames
    init_func=init,
    interval=20,          # ms between frames
    blit=True             # only redraw changed artists (faster)
)

plt.show()

# --- Save (optional) ---
# anim.save('threebody.mp4', writer='ffmpeg', fps=60)
# anim.save('threebody.gif', writer='pillow', fps=30)
```

### Key methods
| Method | What it does |
|---|---|
| `line.set_data(xarr, yarr)` | Update x,y data on a Line2D |
| `line.set_xdata(arr)` | Update just x |
| `ax.set_xlim(lo, hi)` | Axis range |
| `ax.set_aspect('equal')` | Same scale x and y |
| `FuncAnimation(fig, update, frames, interval, blit)` | Core animator |
| `fig.colorbar(mappable)` | If you want energy-colored trails |

### 3D variant
```python
from mpl_toolkits.mplot3d import Axes3D

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
body1, = ax.plot([], [], [], 'o')

def update(frame):
    body1.set_data_3d([x1[frame]], [y1[frame]], [z1[frame]])
    # Note: blit=False required for 3D (blit broken in mpl 3D)
```

---

## Option 2: Pygame (Real-Time)

```python
import pygame
import sys

# --- Init ---
pygame.init()
W, H = 800, 800
screen = pygame.display.set_mode((W, H))
clock = pygame.time.Clock()

# --- Coordinate transform ---
# Physics coords -> pixel coords
def to_pixel(x, y, scale=100, cx=W//2, cy=H//2):
    return int(cx + x * scale), int(cy - y * scale)  # note: y flipped

# --- Colors ---
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED   = (255, 80, 80)
BLUE  = (80, 80, 255)
GREEN = (80, 255, 80)

# --- Trail storage ---
trail1 = []  # list of (px, py) tuples

# --- Main loop ---
running = True
while running:
    # Events
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                running = False
            if event.key == pygame.K_SPACE:
                paused = not paused

    if not paused:
        # === PHYSICS STEP HERE ===
        # e.g. one or more RK4 steps
        # updates positions: r1, r2, r3

        pass

    # --- Draw ---
    screen.fill(BLACK)

    # Trails
    px = to_pixel(r1[0], r1[1])
    trail1.append(px)
    if len(trail1) > 1:
        pygame.draw.lines(screen, RED, False, trail1, 1)

    # Bodies
    pygame.draw.circle(screen, RED,   to_pixel(r1[0], r1[1]), 6)
    pygame.draw.circle(screen, BLUE,  to_pixel(r2[0], r2[1]), 6)
    pygame.draw.circle(screen, GREEN, to_pixel(r3[0], r3[1]), 6)

    # HUD text
    font = pygame.font.SysFont('monospace', 14)
    E_text = font.render(f'E = {energy:.4f}', True, WHITE)
    screen.blit(E_text, (10, 10))

    pygame.display.flip()
    clock.tick(60)  # cap at 60 FPS

pygame.quit()
sys.exit()
```

### Key functions
| Function | What it does |
|---|---|
| `pygame.display.set_mode((W,H))` | Create window |
| `screen.fill(color)` | Clear screen each frame |
| `pygame.draw.circle(surface, color, (x,y), radius)` | Draw body |
| `pygame.draw.lines(surface, color, closed, points, width)` | Draw trail |
| `pygame.draw.line(surface, color, start, end, width)` | Single line |
| `pygame.display.flip()` | Push frame to screen |
| `clock.tick(fps)` | Cap framerate |
| `pygame.event.get()` | Poll input events |
| `font.render(text, antialias, color)` | Create text surface |
| `surface.blit(text_surf, (x,y))` | Draw text to screen |

---

## Physics Side (shared by both)

```python
# State vector: [x1,y1,x2,y2,x3,y3, vx1,vy1,vx2,vy2,vx3,vy3]
# 2D: 12 components total

def derivs(state, masses, G=1.0):
    """Returns d(state)/dt. This is the RHS of Hamilton's equations."""
    r = state[:6].reshape(3, 2)   # positions: shape (3,2)
    v = state[6:].reshape(3, 2)   # velocities: shape (3,2)
    acc = np.zeros_like(r)

    for i in range(3):
        for j in range(3):
            if i != j:
                rij = r[j] - r[i]
                dist = np.linalg.norm(rij)
                acc[i] += G * masses[j] * rij / dist**3

    return np.concatenate([v.flatten(), acc.flatten()])

def rk4_step(state, dt, masses):
    """Single RK4 step."""
    k1 = derivs(state, masses)
    k2 = derivs(state + 0.5*dt*k1, masses)
    k3 = derivs(state + 0.5*dt*k2, masses)
    k4 = derivs(state + dt*k3, masses)
    return state + (dt/6.0) * (k1 + 2*k2 + 2*k3 + k4)

def total_energy(state, masses, G=1.0):
    """Compute total energy (sanity check: should be ~constant)."""
    r = state[:6].reshape(3, 2)
    v = state[6:].reshape(3, 2)
    T = 0.5 * sum(masses[i] * np.dot(v[i], v[i]) for i in range(3))
    V = 0.0
    for i in range(3):
        for j in range(i+1, 3):
            V -= G * masses[i] * masses[j] / np.linalg.norm(r[j] - r[i])
    return T + V
```

### Famous initial conditions (figure-8, equal masses m=1, G=1)

```python
# Chenciner-Montgomery figure-8 solution
masses = np.array([1.0, 1.0, 1.0])

# Positions
r1_0 = np.array([-0.97000436,  0.24308753])
r2_0 = np.array([ 0.97000436, -0.24308753])
r3_0 = np.array([ 0.0,         0.0        ])

# Velocities
v3_0 = np.array([-0.93240737, -0.86473146])
v1_0 = -v3_0 / 2
v2_0 = -v3_0 / 2

state0 = np.concatenate([r1_0, r2_0, r3_0, v1_0, v2_0, v3_0])
```

---

## Sanity checks to run

1. **Energy conservation**: plot `E(t)` — should be flat. Drift means `dt` too large.
2. **Momentum conservation**: $\sum m_i \mathbf{v}_i$ should be constant (zero if COM frame).
3. **Two-body limit**: set $m_3 = 0$ or $m_3 \ll m_1, m_2$. Should recover ellipses.
4. **Symmetry**: equal masses + symmetric IC → trajectories should respect symmetry.
