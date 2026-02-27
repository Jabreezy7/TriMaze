# TriMaze

A first-person 3D maze explorer built with OpenGL. The maze is procedurally generated at runtime using a randomized DFS algorithm, and can be solved automatically using either an exhaustive path search or DFS. The project was built from scratch — including a custom linear algebra library — as a hands-on exploration of real-time 3D graphics programming.

---

## Build Instructions

Make sure you have OpenGL and the required system libraries installed.

To build the project:

```bash
make
```

To run:

```bash
./triangle
```

---

## Platform

* Developed on **macOS**
* Should work on **Linux** / **Windows** with proper OpenGL setup

---

## Controls

| Key | Action |
|-----|--------|
| `Space` | Fly around the maze and descend to the entrance |
| `w` | Move forward |
| `s` | Move backward |
| `a` | Move left |
| `d` | Move right |
| `z` | Look left |
| `x` | Look right |
| `t` | Auto-solve to exit (exhaustive path) |
| `y` | Auto-solve to exit (DFS shortest path) |
| `1` | Toggle ambient lighting |
| `2` | Toggle diffuse lighting |
| `3` | Toggle specular lighting |
| `4` | Toggle lighting effect |
| `5` | Cycle light source: Sun → Viewer → Flashlight |
| `v` | Move sun west → east |
| `b` | Move sun east → west |
| `n` | Move sun north → south |
| `m` | Move sun south → north |
| `q` | Quit |

When using the **flashlight** mode, move your cursor to aim the light beam.

---

## Maze Generation

The maze is an 8×8 grid generated procedurally at startup using a **randomized depth-first search (DFS)**. Each cell tracks which of its four walls (north, east, south, west) are present. The generator begins at cell (0,0), shuffles the four cardinal directions at each step, and recursively carves passages into unvisited neighbors — guaranteeing a perfect maze (no loops, every cell reachable) with a different layout each run.

---

## Maze Solving

Two automatic solvers are available, both triggered from the player's current position inside the maze:

**Exhaustive search (`t`)** — explores all reachable paths and backtracks as needed, recording every step including backtrack moves. This animates the full search process, so you can watch the algorithm feel its way through dead ends before finding the exit.

**DFS solver (`y`)** — uses a standard depth-first search that records only the successful path, discarding dead ends. The result is a clean, direct animation from the player's current cell to the exit.

---

## Camera & Navigation

The camera starts above the maze looking straight down, giving a bird's-eye view. Pressing `Space` triggers a three-phase animated flyby:

1. **Fly To** — the camera arcs from the overhead position to a side-angle vantage point above the entrance.
2. **Fly Around** — the camera orbits 360° around the maze center.
3. **Fly Down** — the camera descends and zooms into the entrance, transitioning into the first-person view.

Once inside the maze, movement is locked to the cardinal grid (north, east, south, west). The player can only move in a direction if the corresponding wall is open, enforcing the maze's structure. All movement — including turning, stepping forward/backward/left/right, and looking left/right — is smoothly animated over a configurable number of steps.

---

## Lighting

Lighting is computed in the fragment shader and supports three independent components that can be toggled on or off independently:

- **Ambient** — constant base illumination, so no surface is ever completely black.
- **Diffuse** — directional shading based on the angle between the surface normal and the light source, giving depth to walls and floors.
- **Specular** — highlights on surfaces facing the light, giving a glossy appearance.

There are three light source modes, cycled with `5`:

- **Sun** — a positional light in the sky, movable along the X and Z axes with `v`/`b`/`n`/`m`. The sun cube object in the scene moves in sync with the light position.
- **Viewer** — the light source is attached to the camera position, illuminating whatever the player looks at.
- **Flashlight** — a spotlight whose aim direction is controlled by the mouse cursor position. Moving the cursor steers the beam in real time.

---

## Textures

All geometry shares a single 800×800 RGB texture atlas loaded from `p2texture04.raw`. Different regions of the atlas are mapped to different object types:

- **Floor tiles** — bottom-left quadrant of the atlas
- **Walls** — center-left region
- **Poles** (corner pillars) — center-right region
- **Spinning cubes** — top-right region (entrance marker uses pole texture; exit marker uses its own spinning cube region)

---

## Animated Objects

Two small cubes float at the maze entrance and exit, spinning continuously in the idle loop. Each one rotates around its own center by translating to the origin, applying a 1°-per-frame Y-axis rotation, and translating back. 

The sun cube also moves when the player presses the sun-control keys, orbiting around the scene origin on the X or Z axis.

---

## Custom Math Library

Rather than relying on GLM or any external math library, all vector and matrix operations are implemented by hand in `myLib.c` / `myLib.h`. This includes:

- `vec4` and `mat4` types stored in column-major order
- Vector operations: add, subtract, scalar multiply, dot product, cross product, normalize, magnitude
- Matrix operations: multiply, transpose, inverse (via cofactor/adjugate), minor, determinant
- Transform constructors: `translate`, `scale`, `rotateX`, `rotateY`, `rotateZ`
- View and projection: `look_at`, `ortho`, `frustum`

---

## Rendering Pipeline & Coordinate Systems

Every frame, each object goes through a standard three-stage transformation pipeline before it reaches the screen:

Each stage is driven by a matrix passed to the vertex shader:

- **CTM (Current Transformation Matrix)** — brings an object from its own local space into world space. Each object (maze, sun, spinning cubes) has its own CTM so they can be transformed independently.
- **Model-View matrix** — produced by `look_at()`, it transforms world-space coordinates into camera/eye space, repositioning the entire scene relative to where the camera is and where it's looking.
- **Projection matrix** — produced by `frustum()`, it maps eye-space coordinates into clip space, applying the perspective divide that makes distant objects appear smaller.

These three matrices are uploaded to the vertex shader as uniforms every frame via `glUniformMatrix4fv`.

---

## Perspective Projection & the Frustum

Perspective projection is what gives the scene its sense of depth — objects shrink as they move away from the camera. It is implemented via a **frustum matrix** constructed in `myLib.c`:

```c
mat4 frustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far);
```

The frustum defines a truncated pyramid-shaped viewing volume. Any geometry inside this volume gets projected onto the near plane and rasterized; anything outside is clipped. The six parameters define the boundaries of this volume:

- `left`, `right`, `bottom`, `top` — the edges of the near clipping plane
- `near`, `far` — depth range of the viewing volume (note: in this project these are stored as negative values following OpenGL's right-hand convention, so `near = -1` and `far = -50`)

During the flyby animation the frustum parameters `left`, `right`, `bottom`, `top`, and `near` are **interpolated over time**, causing the field of view to gradually narrow as the camera descends into the maze. This gives the cinematic "zooming in" effect during the `FLYING_DOWN` phase.

---

## The View Matrix & Look-At Transform

The `look_at()` function constructs the **view matrix** — a rigid body transform that moves the entire world so that the camera sits at the origin, looking down the negative Z axis. It takes three inputs:

- `eyePoint` — where the camera is in world space
- `atPoint` — the point the camera is looking at
- `upVector` — which direction is "up" for the camera (used to prevent roll)

Internally it builds an orthonormal basis from these three vectors:

- `n = normalize(eye - at)` — the direction the camera is looking *away* from (its local Z axis)
- `u = normalize(upVector × n)` — the camera's local X (right) axis
- `v = normalize(n × u)` — the camera's local Y (up) axis

These axes form the rows of a rotation matrix `R`, which is then composed with a translation matrix `T` that moves the world by `-eyePoint`. The final view matrix is `R × T`.

This is computed fresh every frame since the camera moves continuously during animation.

---

## Orthographic Projection

Alongside the perspective frustum, the library also implements **orthographic projection** via:

```c
mat4 ortho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far);
```

Orthographic projection maps the viewing volume to NDC without a perspective divide — parallel lines stay parallel and objects do not shrink with distance. It is implemented as a two-step transform: first translate the center of the viewing box to the origin, then scale each axis so the box spans `[-1, 1]`. While the main renderer uses the perspective frustum, the orthographic function is available in the library and can be swapped in for a top-down map view or UI overlay.

---

## Normal Vectors & Surface Shading

Surface normals are computed on the CPU during initialization, not in the shader. For every triangle (three consecutive vertices), the normal is calculated as the cross product of two edge vectors:

```c
vec4 normal = cross_product(vec_sub(p1, p0), vec_sub(p2, p0));
```

The same flat normal is assigned to all three vertices of the triangle, producing **flat shading** across each face. These normals are uploaded to the GPU in a separate buffer region and accessed in the vertex/fragment shader via the `vNormal` attribute. The shader then uses them for the diffuse and specular lighting calculations.

---

## Geometry Construction & the VAO/VBO Layout

All geometry is built on the CPU as flat arrays of `vec4` positions and `vec2` texture coordinates. Everything — the maze floor, walls, poles, sun, and spinning cubes — is packed into a single interleaved GPU buffer with three sub-regions:

```
[ positions (vec4 × N) | tex coords (vec2 × N) | normals (vec4 × N) ]
```

This buffer is bound to a single VBO, and three vertex attribute pointers are set up with byte offsets to address each region. All objects are drawn from this one buffer; the CTM uniform is swapped between draw calls to transform each object independently:

```c
glUniformMatrix4fv(ctm_location, ..., &maze_ctm);
glDrawArrays(GL_TRIANGLES, 0, num_vertices_maze);

glUniformMatrix4fv(ctm_location, ..., &sun_ctm);
glDrawArrays(GL_TRIANGLES, num_vertices_maze, num_vertices_sun);
// ...and so on
```

Depth testing (`GL_DEPTH_TEST`) is enabled so closer surfaces correctly occlude distant ones, and `glDepthRange(1, 0)` reverses the depth buffer direction to match the negative `near`/`far` convention used in the projection matrix.

---

## Rotation & the Spinning Cube Transform

Local-origin rotation (spinning a cube around its own center rather than the world origin) requires a **translate → rotate → un-translate** sequence. The spinning cubes apply this every frame in the idle callback:

```c
spinning_exit_ctm = translate(3.5, 0, 3.5)
                  × rotateY(1°)
                  × translate(-3.5, 0, -3.5)
                  × prevSpinExit;
```

Reading right to left: move the cube's center to the world origin, rotate 1° around Y, move it back, then compose with the accumulated transform from all previous frames. Without the un-translate step, the cube would orbit around the world origin instead of spinning in place.

The same pattern is used for sun movement — `rotateX` or `rotateZ` is applied around the world origin (which is the sun's intended orbit center), so the sun visibly arcs across the sky.

---

This project was created for learning and experimentation with OpenGL and low-level graphics programming.
