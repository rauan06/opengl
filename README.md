# OpenGL Assignment 1 - 2D Graphics Programming

## Overview
This assignment demonstrates OpenGL programming with modern OpenGL (OpenGL 3.3+) using vertex and fragment shaders. The programs create various 2D geometric shapes with color interpolation and shading effects.

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

## Programs Included

### Task 1: Basic Shapes
1. **red_triangle** - Displays a single red triangle using GL_TRIANGLES
2. **blue_square** - Displays a single blue square using GL_TRIANGLE_STRIP
3. **Modified red_triangle** - Modified to display a blue square (as required)

### Task 2: Complex Shapes
1. **task2_part1** - Creates a complex composition with:
   - Red ellipse (top left) - using parametric equations with y-axis scaled to 60%
   - Color-interpolated triangle (top center) - red, green, blue vertices
   - Shaded red circle (top right) - with radial shading effect
   - Nested black and white squares (bottom center) - 4 concentric squares

2. **task2_part2** - Creative geometric art featuring:
   - Golden star (top left) - 5-pointed star with filled triangles
   - Color-gradient hexagon (top right) - purple center to cyan edges
   - Rainbow spiral (bottom center) - logarithmic spiral with color variation

## Compilation and Usage

### Build All Programs
```bash
make all
```

### Build Individual Programs
```bash
make red_triangle
make blue_square
make task2_part1
make task2_part2
```

### Run Programs
```bash
./red_triangle
./blue_square
./task2_part1
./task2_part2
```

### Clean Build Files
```bash
make clean
```

## Technical Implementation

### Shader Architecture
All programs use modern OpenGL with:
- Vertex shader for position and color attributes
- Fragment shader for color interpolation
- VAO (Vertex Array Objects) and VBO (Vertex Buffer Objects)

### Key Features
- **Color Interpolation**: Vertex colors are interpolated across triangle surfaces
- **Parametric Shapes**: Circles and ellipses use trigonometric parametric equations
- **Gradient Effects**: Smooth color transitions using vertex attribute interpolation
- **Modern OpenGL**: No deprecated functions, uses shader-based rendering

### Shape Generation
- **Ellipse**: `x = centerX + radiusX * cos(angle)`, `y = centerY + radiusY * sin(angle)`
- **Circle Shading**: Red intensity varies as `0.5 + 0.5 * cos(angle)`
- **Spiral**: Logarithmic spiral with rainbow color mapping
- **Star**: Alternating inner/outer radius points for 5-pointed star

## File Structure
```
c_plus/
├── Makefile              # Build configuration
├── README.md             # This file
├── src/
│   ├── red_triangle.cpp  # Task 1: Red triangle
│   ├── blue_square.cpp   # Task 1: Blue square
│   ├── task2_part1.cpp   # Task 2: Complex shapes
│   └── task2_part2.cpp   # Task 2: Creative shapes
└── report.md             # Implementation report
```

## Testing
All programs have been tested on:
- **Operating System**: Arch Linux (6.16.8-arch3-1)
- **Compiler**: GCC with C++11 standard
- **OpenGL Version**: 3.3+ (core profile)
- **Window Size**: 500x500 pixels

## Controls
- **ESC**: Close program (standard GLUT behavior)
- **Window Resize**: Programs adapt to window size changes

## Notes
- All programs use normalized device coordinates (-1 to 1)
- Colors are specified in RGB format (0.0 to 1.0)
- Programs are designed to be cross-platform compatible
- No deprecated OpenGL functions are used
