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
unsigned int starVAO, starVBO;
unsigned int hexagonVAO, hexagonVBO;
unsigned int spiralVAO, spiralVBO;

void createStar(float centerX, float centerY, float outerRadius, float innerRadius, const std::vector<float>& colors)
{
    std::vector<float> vertices;
    int points = 5;
    
    for (int i = 0; i <= points * 2; i++)
    {
        float angle = M_PI * i / points;
        float radius = (i % 2 == 0) ? outerRadius : innerRadius;
        float x = centerX + radius * cos(angle - M_PI/2);
        float y = centerY + radius * sin(angle - M_PI/2);
        
        vertices.insert(vertices.end(), {x, y, 0.0f, colors[0], colors[1], colors[2]});
    }
    
    glGenVertexArrays(1, &starVAO);
    glGenBuffers(1, &starVBO);
    
    glBindVertexArray(starVAO);
    glBindBuffer(GL_ARRAY_BUFFER, starVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void createHexagon(float centerX, float centerY, float radius, const std::vector<float>& centerColor, const std::vector<float>& edgeColor)
{
    std::vector<float> vertices;
    int sides = 6;
    
    // Center vertex
    vertices.insert(vertices.end(), {centerX, centerY, 0.0f, centerColor[0], centerColor[1], centerColor[2]});
    
    for (int i = 0; i <= sides; i++)
    {
        float angle = 2.0f * M_PI * i / sides;
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        
        // Interpolate between center and edge colors
        float t = (float)i / sides;
        float r = centerColor[0] + t * (edgeColor[0] - centerColor[0]);
        float g = centerColor[1] + t * (edgeColor[1] - centerColor[1]);
        float b = centerColor[2] + t * (edgeColor[2] - centerColor[2]);
        
        vertices.insert(vertices.end(), {x, y, 0.0f, r, g, b});
    }
    
    glGenVertexArrays(1, &hexagonVAO);
    glGenBuffers(1, &hexagonVBO);
    
    glBindVertexArray(hexagonVAO);
    glBindBuffer(GL_ARRAY_BUFFER, hexagonVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void createSpiral(float centerX, float centerY, float maxRadius, int turns)
{
    std::vector<float> vertices;
    int segments = turns * 100;
    
    for (int i = 0; i < segments; i++)
    {
        float t = (float)i / segments;
        float angle = 2.0f * M_PI * turns * t;
        float radius = maxRadius * t;
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        
        // Rainbow color based on angle
        float r = 0.5f + 0.5f * sin(angle);
        float g = 0.5f + 0.5f * sin(angle + 2.0f * M_PI / 3.0f);
        float b = 0.5f + 0.5f * sin(angle + 4.0f * M_PI / 3.0f);
        
        vertices.insert(vertices.end(), {x, y, 0.0f, r, g, b});
    }
    
    glGenVertexArrays(1, &spiralVAO);
    glGenBuffers(1, &spiralVBO);
    
    glBindVertexArray(spiralVAO);
    glBindBuffer(GL_ARRAY_BUFFER, spiralVBO);
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
    
    // Draw star (top left)
    glBindVertexArray(starVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 11); // 5 points * 2 + 1
    
    // Draw hexagon (top right)
    glBindVertexArray(hexagonVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 8); // 6 sides + center + 1 for closure
    
    // Draw spiral (bottom center)
    glBindVertexArray(spiralVAO);
    glDrawArrays(GL_LINE_STRIP, 0, 300); // 3 turns * 100 segments
    
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
    
    // Create creative shapes
    std::vector<float> goldColor = {1.0f, 0.84f, 0.0f};
    createStar(-0.4f, 0.4f, 0.4f, 0.08f, goldColor); // Golden star
    
    std::vector<float> centerColor = {0.5f, 0.0f, 1.0f}; // Purple center
    std::vector<float> edgeColor = {0.0f, 1.0f, 1.0f};   // Cyan edges
    createHexagon(0.4f, 0.4f, 0.4f, centerColor, edgeColor); // Color-gradient hexagon
    
    createSpiral(0.0f, -0.3f, 0.4f, 3); // Rainbow spiral
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Task 2 Part 2 - Creative Geometric Art");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    
    glutMainLoop();
    
    return 0;
}
