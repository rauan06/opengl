# Assignment 4: Bezier Patches, Picking, and Texture Mapping

## Overview

This assignment implements three parts covering advanced OpenGL topics:
1. **Part 1**: Bezier Patch with interactive control points
2. **Part 2**: Anti-aliasing and Picking using Frame Buffer Objects
3. **Part 3**: Texture Mapping (2D and 3D procedural)

## Requirements

- Linux operating system
- GCC compiler with C++11 support
- OpenGL development libraries
- GLEW (OpenGL Extension Wrangler Library)
- GLUT (OpenGL Utility Toolkit)

## Installation Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get install build-essential libgl1-mesa-dev libglew-dev freeglut3-dev
```

On Arch Linux:
```bash
sudo pacman -S base-devel glew freeglut
```

## Part 1: Bezier Patch

### Features
- Bicubic Bezier patch evaluation (iterative method)
- Flat-shaded tessellation with Phong shading
- Interactive control point modification
- Adjustable tessellation resolution (10x10 initial)
- Control points visualization (red, selected point in yellow)
- X, Y, Z axes visualization (red, green, blue)

### Controls
- **0-9, A-F**: Select control point (0-15)
- **X/x**: Decrease/Increase X position of selected point
- **Y/y**: Decrease/Increase Y position of selected point
- **Z/z**: Decrease/Increase Z position of selected point
- **+/-**: Increase/Decrease tessellation resolution
- **Arrow keys**: Rotate camera
- **R/r**: Reset camera view
- **ESC**: Exit

### Implementation Details
- Uses iterative Bezier basis function evaluation (not recursive)
- Computes surface normals using cross product of partial derivatives
- Phong shading with ambient, diffuse, and specular components
- Light source attached to camera

### Build and Run
```bash
make assignment4_part1
./assignment4_part1
```

## Part 2: Anti-aliasing and Picking

### Features
- Three smooth-shaded triangle-mesh objects
- Perspective projection
- Frame Buffer Object (FBO) for picking
- Anti-aliasing toggle
- Click objects to change color (random)

### Controls
- **Left click**: Select object (changes to random color)
- **A/a**: Toggle anti-aliasing
- **Arrow keys**: Rotate camera
- **R/r**: Reset camera view
- **ESC**: Exit

### Implementation Details
- Picking implemented using FBO with unique constant colors
- Each object rendered with unique color in picking pass
- Mouse click reads pixel color from FBO to identify object
- Anti-aliasing using GLUT_MULTISAMPLE

### Build and Run
```bash
make assignment4_part2
./assignment4_part2
```

## Part 3a: Image-based Texture Mapping on Bezier Patch

### Features
- 12x12 triangle mesh from Bezier patch
- Procedurally generated 2D texture
- Texture coordinates same as (u,v) parameters
- Phong shading with texture as diffuse color

### Controls
- **Arrow keys**: Rotate camera
- **R/r**: Reset camera view
- **ESC**: Exit

### Implementation Details
- Procedural texture generation (spiral/wave pattern)
- Texture applied in fragment shader
- Texture coordinates mapped from Bezier patch (u,v) parameters

### Build and Run
```bash
make assignment4_part3a
./assignment4_part3a
```

## Part 3b: 3D Procedural Texturing

### Features
- 3D procedural texture computed from world coordinates
- Applied to torus mesh (can be extended to SMF models)
- Interesting pattern combining multiple wave functions
- Phong shading with 3D texture as diffuse color

### Controls
- **Arrow keys**: Rotate camera
- **R/r**: Reset camera view
- **ESC**: Exit

### Implementation Details
- 3D texture computed in fragment shader using world coordinates
- Combines radial waves, spiral patterns, and 3D noise-like patterns
- Texture color varies based on (X, Y, Z) position in world space

### Build and Run
```bash
make assignment4_part3b
./assignment4_part3b
```

## Build All Programs

```bash
make all
```

Or build individually:
```bash
make assignment4_part1
make assignment4_part2
make assignment4_part3a
make assignment4_part3b
```

## Code Structure

```
src/
├── assignment4_part1_bezier.cpp      # Part 1: Bezier Patch
├── assignment4_part2_picking.cpp     # Part 2: Anti-aliasing and Picking
├── assignment4_part3a_texture_bezier.cpp  # Part 3a: 2D Texture on Bezier
└── assignment4_part3b_3d_texture.cpp # Part 3b: 3D Procedural Texture
```

## Technical Implementation

### Bezier Patch Evaluation
- Uses iterative evaluation of Bezier basis functions
- Basis functions: B₀(t) = (1-t)³, B₁(t) = 3t(1-t)², B₂(t) = 3t²(1-t), B₃(t) = t³
- Surface normals computed from partial derivatives (du and dv)

### Phong Shading Model
- Ambient: I_a = k_a * I_light * color
- Diffuse: I_d = k_d * (N·L) * I_light * color
- Specular: I_s = k_s * (R·V)^n * I_light
- Combined: I = I_a + I_d + I_s

### Frame Buffer Objects (FBO)
- Used for off-screen rendering in picking
- Color texture attachment for reading pixel colors
- Depth renderbuffer for depth testing

### Texture Mapping
- **2D Texture**: Procedurally generated pattern, applied using (u,v) coordinates
- **3D Texture**: Computed from world coordinates (X, Y, Z) in fragment shader

## Notes

- All programs use modern OpenGL (OpenGL 3.3+)
- Shaders written in GLSL 330 core
- No deprecated OpenGL functions used
- Control points are hardcoded in Part 1 (can be extended to load from file)
- Part 3b uses a torus mesh (can be extended to load SMF files)

## Libraries Used

- **OpenGL**: Graphics rendering
- **GLEW**: OpenGL extension loading
- **GLUT**: Window management and input handling
- **Standard C++ libraries**: Vector, cmath, iostream

## Testing

All programs have been tested on:
- **Operating System**: Arch Linux
- **Compiler**: GCC with C++11 standard
- **OpenGL Version**: 3.3+ (core profile)
- **Window Size**: 800x600 pixels

## Controls Summary

### Common Controls (all parts)
- **Arrow keys**: Rotate camera
- **R/r**: Reset camera view
- **ESC**: Exit program

### Part 1 Specific
- **0-9, A-F**: Select control point
- **X/x, Y/y, Z/z**: Modify control point position
- **+/-**: Change tessellation resolution

### Part 2 Specific
- **Left click**: Select object
- **A/a**: Toggle anti-aliasing

## Future Enhancements

- Load control points from file
- Load SMF models for Part 3b
- Save/load texture images
- More sophisticated 3D texture patterns
- Multiple light sources
- Shadow mapping

