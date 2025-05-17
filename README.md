
```
      ___.        __                                          
  ____\_ |__     |__| ____  __ _________  ______ ____   ______
 /  _ \| __ \    |  |/ ___\|  |  \_  __ \/  ___// __ \ /  ___/
(  <_> ) \_\ \   |  \  \___|  |  /|  | \/\___ \\  ___/ \___ \ 
 \____/|___  /\__|  |\___  >____/ |__|  /____  >\___  >____  >
           \/\______|    \/                  \/     \/     \/ 
```

**objcurses** is a minimalistic 3D object viewer that runs in your terminal using `ncurses`. It renders `.obj` models in real time using ASCII characters and a simple rendering pipeline. The project was built from scratch in modern C++20 using up-to-date best practices and a clean modular design, as a personal exploration of low-level graphics programming - without relying on external graphic engines or frameworks.

<p align="center">
  <img src="resources/images/demo.gif" alt="TUI Demo Gif" width="600">
</p>

# Features

- Render `.obj` files directly in terminal
- Real-time camera and directional light control
- Basic color support from `.mtl` material files
- HUD overlay for additional stats
- Minimal dependencies: C/C++, `ncurses`, math

# Use Cases

* Preview 3D files instantly without launching heavy editors
* Generate custom ASCII art for neofetch or terminal splash
* Style CLI tools or games with ASCII-based intros and visuals


# Usage

```bash
objcurses [OPTIONS] <file.obj>
```

## Options

```
-c, --color        Enable colors from .mtl file
-l, --light        Disable light rotation
-h, --help         Print help
-v, --version      Print version
```

Examples:

```bash
objcurses file.obj          # basic
objcurses -c file.obj       # enable colors
objcurses --light file.obj  # disable light rotation

```

## Controls

Supports arrow keys, WASD, and Vim-style navigation:

```
←, h, a            Rotate left
→, l, d            Rotate right
↑, k, w            Rotate up
↓, j, s            Rotate down
+, i               Zoom in
-, o               Zoom out
Tab                Toggle HUD
q                  Quit
```

# Build

CMake setup is already configured - just clone and build:

```bash
git clone https://github.com/admtrv/objcurses
cd objcurses
mkdir build && cd build
cmake ..
make
```

# References

## Inspirations

* [Codeology](http://codeology.kunstu.com/)
  The seed of an idea. Codeology visualizes GitHub repositories as abstract 3D shapes made from symbols. This inspired me to create an ASCII-based 3D renderer from scratch.

* [Donut math (a1k0n)](https://www.a1k0n.net/2011/07/20/donut-math.html)
  Cool article that breaks down the logic of the classic `donut.c` - a rotating ASCII torus in terminal using C. A great example of terminal 3D rendering and a key resource for understanding core rendering math.

* [3D ASCII Viewer (autopawn)](https://github.com/autopawn/3d-ascii-viewer)
  Viewer of 3D models in ASCII, written in C. I treated it as a logical predecessor to my project - it helped me explore how more complex rendering math could work.

## Resources

* [Data Types (OpenGL Documentation)](https://www.khronos.org/opengl/wiki/Data_Type_%28GLSL%29)
  Used to understand standard OpenGL types like `vec3`, etc.

* [Polygon triangulation (Wikipedia)](https://en.wikipedia.org/wiki/Polygon_triangulation)
  For correctly converting complex polygon shapes into triangles for rendering.

* [OBJ Parsing (Stack Overflow)](https://stackoverflow.com/questions/52824956/how-can-i-parse-a-simple-obj-file-into-triangles)
  Clarified parsing of `.obj` files and preparing vertex data.

## Sample Models

* [Fox Model (PixelMannen)](https://opengameart.org/content/fox-and-shiba) was used throughout development for testing `.obj` and `.mtl` parsing and rendering. The files `fox.obj` and `fox.mtl` are located in `/resources/objects/`, and the same model was featured in the recorded demo footage.

* [Low Poly Tree (kiprus)](https://free3d.com/3d-model/low_poly_tree-816203.html) played a key role in identifying a flaw in the triangulation algorithm, as it contains complex non-convex polygons that exposed edge cases in ear clipping algorithm.

* [Linux Mascot (Vido89)](https://blendswap.com/blend/23774) model help in fixing triangulation logic by triggering false degenerate cases due to its irregular normals and detailed geometry.
