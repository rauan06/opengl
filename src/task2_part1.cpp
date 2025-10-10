#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <vector>

// Shader source code
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    vertexColor = aColor;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexColor, 1.0);
}
)";

// Global variables
unsigned int shaderProgram;
unsigned int ellipseVAO, ellipseVBO;
unsigned int triangleVAO, triangleVBO;
unsigned int circleVAO, circleVBO;
unsigned int squaresVAO, squaresVBO;

void createEllipse(float centerX, float centerY, float radiusX, float radiusY, const std::vector<float>& colors)
{
    std::vector<float> vertices;
    int segments = 50;
    
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * M_PI * i / segments;
        float x = centerX + radiusX * cos(angle);
        float y = centerY + radiusY * sin(angle);
        
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
        
        // Red color for ellipse
        vertices.push_back(colors[0]);
        vertices.push_back(colors[1]);
        vertices.push_back(colors[2]);
    }
    
    glGenVertexArrays(1, &ellipseVAO);
    glGenBuffers(1, &ellipseVBO);
    
    glBindVertexArray(ellipseVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ellipseVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void createColorTriangle(float centerX, float centerY, float size)
{
    std::vector<float> vertices;
    
    // Triangle vertices with different colors
    // Top vertex - red
    vertices.insert(vertices.end(), {centerX, centerY + size, 0.0f, 1.0f, 0.0f, 0.0f});
    // Bottom left - green
    vertices.insert(vertices.end(), {centerX - size, centerY - size, 0.0f, 0.0f, 1.0f, 0.0f});
    // Bottom right - blue
    vertices.insert(vertices.end(), {centerX + size, centerY - size, 0.0f, 0.0f, 0.0f, 1.0f});
    
    glGenVertexArrays(1, &triangleVAO);
    glGenBuffers(1, &triangleVBO);
    
    glBindVertexArray(triangleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void createShadedCircle(float centerX, float centerY, float radius)
{
    std::vector<float> vertices;
    int segments = 50;
    
    // Center vertex (darker red)
    vertices.insert(vertices.end(), {centerX, centerY, 0.0f, 0.3f, 0.0f, 0.0f});
    
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * M_PI * i / segments;
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        
        // Vary red color based on angle for shading effect
        float redIntensity = 0.5f + 0.5f * sin(-(angle - M_PI/2 - 10));
        vertices.insert(vertices.end(), {x, y, 0.0f, redIntensity, 0.0f, 0.0f});
    }
    
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);
    
    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void createNestedSquares(float centerX, float centerY, float size)
{
    std::vector<float> vertices;
    
    // Create 6 nested squares (white, black, white, black, white, black from outside to inside)
    float sizes[] = {size, size*0.83f , size * 0.66f, size * 0.49f, size * 0.32f, size * 0.15f};
    float colors[][3] = {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
    
    for (int s = 0; s < 6; s++)
    {
        float currentSize = sizes[s];
        float halfSize = currentSize / 2.0f;
        
        // Square vertices using GL_TRIANGLE_STRIP
        // Bottom left
        vertices.insert(vertices.end(), {centerX - halfSize, centerY - halfSize, 0.0f, colors[s][0], colors[s][1], colors[s][2]});
        // Bottom right
        vertices.insert(vertices.end(), {centerX + halfSize, centerY - halfSize, 0.0f, colors[s][0], colors[s][1], colors[s][2]});
        // Top left
        vertices.insert(vertices.end(), {centerX - halfSize, centerY + halfSize, 0.0f, colors[s][0], colors[s][1], colors[s][2]});
        // Top right
        vertices.insert(vertices.end(), {centerX + halfSize, centerY + halfSize, 0.0f, colors[s][0], colors[s][1], colors[s][2]});
    }
    
    glGenVertexArrays(1, &squaresVAO);
    glGenBuffers(1, &squaresVBO);
    
    glBindVertexArray(squaresVAO);
    glBindBuffer(GL_ARRAY_BUFFER, squaresVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    
    // Draw ellipse (top left)
    glBindVertexArray(ellipseVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 51); // 50 segments + 1 for closure
    
    // Draw color triangle (top center)
    glBindVertexArray(triangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    // Draw shaded circle (top right)
    glBindVertexArray(circleVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 52); // 50 segments + center + 1 for closure
    
    // Draw nested squares (bottom center)
    glBindVertexArray(squaresVAO);
    for (int i = 0; i < 6; i++)
    {
        glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
    }
    
    glBindVertexArray(0);
    glutSwapBuffers();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}

unsigned int compileShader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    
    return shader;
}

void init()
{
    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }
    
    // Compile shaders
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    // Create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
    }
    
    // Delete shaders as they're now linked
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Create shapes
    std::vector<float> redColor = {1.0f, 0.0f, 0.0f};
    createEllipse(-0.6f, 0.75f, 0.2f, 0.1f, redColor); // Red ellipse (scaled y to 60%)
    createColorTriangle(0.0f, 0.75f, 0.25f); // Color-interpolated triangle
    createShadedCircle(0.6f, 0.75f, 0.2f); // Shaded red circle
    createNestedSquares(0.0f, -0.2f, 1.2f); // Nested squares
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Task 2 Part 1 - Complex Shapes");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    
    glutMainLoop();
    
    return 0;
}
