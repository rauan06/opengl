# OpenGL Assignment 1 - Implementation Report

## Task 1: Getting Started

### Program 1: Red Triangle (`red_triangle.cpp`)
**Implementation Steps:**
1. **OpenGL Setup**: Initialized GLUT, GLEW, and created a 500x500 window
2. **Shader Creation**: 
   - Vertex shader: Handles position and color attributes using `vPosition` and `vColor`
   - Fragment shader: Outputs interpolated vertex colors
3. **Triangle Definition**: Created 3 vertices with red color (1.0, 0.0, 0.0)
4. **Rendering**: Used `GL_TRIANGLES` primitive to draw the triangle

**Key Features:**
- Modern OpenGL 3.3+ with shader-based rendering
- VAO/VBO for efficient vertex data management
- Color interpolation across triangle surface

### Program 2: Blue Square (`blue_square.cpp`)
**Implementation Steps:**
1. **Square Geometry**: Defined 4 vertices forming a square using normalized coordinates
2. **Color Assignment**: All vertices colored blue (0.0, 0.0, 1.0)
3. **Rendering Method**: Used `GL_TRIANGLE_STRIP` for efficient square rendering
4. **Window Setup**: Same 500x500 window configuration as triangle program

### Program 3: Modified Red Triangle (Task Requirement)
**Modifications Made:**
1. **Line 1**: Changed window title from "Red Triangle" to "Blue Square"
2. **Line 2**: Replaced triangle vertices with square vertices
3. **Line 3**: Changed drawing mode from `GL_TRIANGLES` to `GL_TRIANGLE_STRIP`

**Result**: The modified program now displays a blue square that fits inside the window, meeting the requirement of "about 3 lines of code changes."

## Task 2: Drawing 2D Polygons with Color

### Part 1: Complex Shape Composition (`task2_part1.cpp`)

#### Red Ellipse (Top Left)
**Implementation:**
- Used parametric equations: `x = centerX + radiusX * cos(angle)`, `y = centerY + radiusY * sin(angle)`
- Scaled Y-axis to 60% for elliptical shape: `radiusY = 0.6 * radiusX`
- Generated 50 segments for smooth curve
- Rendered using `GL_TRIANGLE_FAN` with center vertex

#### Color-Interpolated Triangle (Top Center)
**Implementation:**
- Three vertices with distinct colors:
  - Top vertex: Red (1.0, 0.0, 0.0)
  - Bottom left: Green (0.0, 1.0, 0.0)
  - Bottom right: Blue (0.0, 0.0, 1.0)
- OpenGL automatically interpolates colors across triangle surface
- Rendered using `GL_TRIANGLES`

#### Shaded Red Circle (Top Right)
**Implementation:**
- Generated circle using parametric equations with 50 segments
- Center vertex with dark red color (0.3, 0.0, 0.0)
- Edge vertices with varying red intensity: `0.5 + 0.5 * cos(angle)`
- Creates radial shading effect from dark center to bright edges
- Rendered using `GL_TRIANGLE_FAN`

#### Nested Black and White Squares (Bottom Center)
**Implementation:**
- Created 4 concentric squares with decreasing sizes
- Alternating colors: white, black, white, black
- Each square rendered as `GL_TRIANGLE_STRIP`
- Size progression: 100%, 80%, 60%, 40% of original size

### Part 2: Creative Geometric Art (`task2_part2.cpp`)

#### Golden Star (Top Left)
**Implementation:**
- 5-pointed star using alternating inner/outer radius points
- Outer radius: 0.15, Inner radius: 0.08
- Golden color: (1.0, 0.84, 0.0)
- Rendered using `GL_TRIANGLE_FAN` with 11 vertices (5 points × 2 + center)

#### Color-Gradient Hexagon (Top Right)
**Implementation:**
- 6-sided polygon with center vertex
- Color interpolation from purple center (0.5, 0.0, 1.0) to cyan edges (0.0, 1.0, 1.0)
- Linear color interpolation based on vertex index
- Rendered using `GL_TRIANGLE_FAN`

#### Rainbow Spiral (Bottom Center)
**Implementation:**
- Logarithmic spiral using parametric equations
- 3 turns with 300 segments total
- Rainbow color mapping using sine functions:
  - Red: `0.5 + 0.5 * sin(angle)`
  - Green: `0.5 + 0.5 * sin(angle + 2π/3)`
  - Blue: `0.5 + 0.5 * sin(angle + 4π/3)`
- Rendered using `GL_LINE_STRIP` for spiral effect

## Technical Implementation Details

### Shader Architecture
All programs use identical shader setup:
- **Vertex Shader**: Processes position and color attributes
- **Fragment Shader**: Outputs final pixel colors with interpolation
- **Shader Compilation**: Error checking and linking validation

### Modern OpenGL Features
- **VAO/VBO**: Efficient vertex data management
- **Attribute Locations**: Explicit attribute binding (location 0 for position, location 1 for color)
- **No Deprecated Functions**: All rendering uses modern OpenGL 3.3+ features

### Coordinate System
- Normalized device coordinates (-1.0 to 1.0)
- Centered shapes with appropriate scaling
- 500x500 pixel window with automatic viewport adjustment

### Build System
- **Makefile**: Automated compilation with dependency management
- **GLEW Integration**: OpenGL extension loading
- **Cross-platform**: Compatible with Linux distributions

## Challenges and Solutions

### Challenge 1: Shader Compilation
**Problem**: Initial shader compilation errors due to version compatibility
**Solution**: Used explicit `#version 330 core` and proper attribute location binding

### Challenge 2: Color Interpolation
**Problem**: Ensuring smooth color transitions across shapes
**Solution**: Used vertex color attributes with OpenGL's automatic interpolation

### Challenge 3: Shape Positioning
**Problem**: Positioning shapes to match the reference image layout
**Solution**: Used normalized coordinates with careful offset calculations

### Challenge 4: Spiral Rendering
**Problem**: Creating smooth logarithmic spiral with proper color mapping
**Solution**: Used parametric equations with angle-based color functions

## Performance Considerations
- Efficient vertex buffer usage with static data
- Minimal draw calls per frame
- Proper VAO binding to reduce state changes
- Optimized segment counts for smooth curves without excessive geometry

## Future Enhancements
- Interactive controls for shape manipulation
- Animation capabilities
- Additional geometric shapes
- Texture mapping support
- Shader uniform variables for dynamic effects

## Conclusion
All programs successfully demonstrate modern OpenGL programming techniques with proper shader usage, efficient vertex data management, and creative geometric rendering. The implementation follows OpenGL best practices and avoids deprecated functions while achieving the required visual results.
