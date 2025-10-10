# OpenGL Assignment - Implementation Report

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
- Edge vertices with varying red intensity: `0.5 + 0.5 * sin(-(angle - π/2 - 10))`
- Creates complex shading effect with offset and phase shift
- Rendered using `GL_TRIANGLE_FAN`

#### Nested Black and White Squares (Bottom Center)
**Implementation:**
- Created 6 concentric squares with decreasing sizes
- Alternating colors: white, black, white, black, white, black
- Each square rendered as `GL_TRIANGLE_STRIP`
- Size progression: 100%, 83%, 66%, 49%, 32%, 15% of original size

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

## Task 3: 3D Colored Cube with Interactive Transformations (`task3_3d_cube.cpp`)

### Overview
A comprehensive 3D OpenGL program that displays a colored cube with interactive transformation controls. The program implements all required features including depth testing, transformation matrices, keyboard controls, and proper 3D rendering.

### Cube Geometry and Color
**Implementation:**
- **Cube Structure**: Standard 3D cube with 8 vertices defining a unit cube centered at origin
- **RGB Color Mapping**: Each vertex assigned distinct RGB colors for gradient effects:
  - Red (1.0, 0.0, 0.0) at bottom-left vertices
  - Green (0.0, 1.0, 0.0) at bottom-right vertices  
  - Blue (0.0, 0.0, 1.0) at top vertices
  - Yellow (1.0, 1.0, 0.0) at top-left vertices
  - Magenta (1.0, 0.0, 1.0) at back vertices
  - Cyan (0.0, 1.0, 1.0) at back-top vertices
- **Face Rendering**: 36 vertices total (6 faces × 6 vertices per face using GL_TRIANGLES)
- **Color Interpolation**: Smooth color gradients across faces via vertex color attributes

### 3D Rendering Features
**Depth Testing:**
- Enabled `GL_DEPTH_TEST` with `GL_LESS` depth function
- Proper depth buffer clearing in display loop
- Ensures correct 3D object visibility and occlusion

**Transformation System:**
- **Matrix-based Transformations**: Custom matrix operations for scale, rotation, and translation
- **Transformation Order**: Scale → Rotation → Translation (as specified)
- **Matrix Multiplication**: Manual 4x4 matrix multiplication for combining transformations
- **Shader Integration**: Transformation matrix passed as uniform to vertex shader

### Interactive Controls

#### Transformation Selection Menu
- **'S'** - Select SCALE transformation mode
- **'R'** - Select ROTATE transformation mode  
- **'T'** - Select TRANSLATE transformation mode
- Visual feedback via terminal output

#### Component Modification (X, Y, Z)
- **'Q'/'A'** - Decrease/Increase X component
- **'W'/'Z'** - Decrease/Increase Y component
- **'E'/'D'** - Decrease/Increase Z component
- Real-time transformation updates with each keystroke

#### Delta Controls
- **'-'/'+'** - Decrease/Increase transformation delta
- Separate deltas for scale (0.1), rotation (10°), and translation (0.1)
- Dynamic delta adjustment affects transformation sensitivity

#### Additional Controls
- **'0'** - Reset all transformations to default values
- **ESC** - Exit program
- Comprehensive terminal instructions displayed at startup

### Technical Implementation

#### Shader Architecture
**Vertex Shader:**
```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
uniform mat4 transform;
out vec3 vertexColor;
void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    vertexColor = aColor;
}
```

**Fragment Shader:**
```glsl
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(vertexColor, 1.0);
}
```

#### Matrix Operations
- **Identity Matrix Creation**: Proper 4x4 identity matrix initialization
- **Scale Matrix**: Diagonal matrix with scale factors
- **Rotation Matrices**: Separate X, Y, Z rotation matrices using trigonometric functions
- **Translation Matrix**: 4x4 matrix with translation values in bottom row
- **Matrix Multiplication**: Manual 4x4 matrix multiplication for transformation combination

#### State Management
- Global transformation state variables for scale, rotation, and translation
- Current transformation mode tracking
- Delta value management for each transformation type
- Real-time matrix updates on user input

### Visual Features
- **Smooth Color Gradients**: RGB color interpolation across cube faces
- **3D Perspective**: Proper depth perception with depth testing
- **Real-time Updates**: Immediate visual feedback for all transformations
- **Default Orthographic Projection**: As specified, no camera/projection transformations

### Build Integration
- **Makefile Target**: `task3_3d_cube` for easy compilation
- **Dependencies**: GLEW, GLUT, OpenGL libraries
- **Cross-platform**: Compatible with Linux OpenGL implementations

## Conclusion
All programs successfully demonstrate modern OpenGL programming techniques with proper shader usage, efficient vertex data management, and creative geometric rendering. The 3D cube program specifically showcases advanced 3D graphics concepts including transformation matrices, depth testing, and interactive 3D manipulation. The implementation follows OpenGL best practices and avoids deprecated functions while achieving the required visual results.
