#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <random>

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
int mainWindow, subWindow, window2;
bool animationEnabled = true;
float rotationAngle = 0.0f;
float breathingScale = 1.0f;
float breathingDirection = 1.0f;

// Color for circle and triangle in window2
float window2Color[3] = {1.0f, 0.0f, 0.0f}; // Start with red

// Subwindow background color
float subWindowBgColor[3] = {0.2f, 0.3f, 0.8f}; // Blue background

// VAOs and VBOs
unsigned int squaresVAO, squaresVBO;
unsigned int ellipseVAO, ellipseVBO;
unsigned int circleVAO, circleVBO;
unsigned int triangleVAO, triangleVBO;
std::vector<unsigned int> breathingCirclesVAO, breathingCirclesVBO;
std::vector<float> breathingCirclesData; // Store circle positions and colors

void createNestedSquares()
{
    std::vector<float> vertices;
    
    // Create 4 nested squares (white, black, white, black from outside to inside)
    float sizes[] = {0.3f, 0.24f, 0.18f, 0.12f};
    float colors[][3] = {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
    
    for (int s = 0; s < 4; s++)
    {
        float currentSize = sizes[s];
        float halfSize = currentSize / 2.0f;
        
        // Square vertices using GL_TRIANGLE_STRIP
        vertices.insert(vertices.end(), {-halfSize, -halfSize, 0.0f, colors[s][0], colors[s][1], colors[s][2]});
        vertices.insert(vertices.end(), {halfSize, -halfSize, 0.0f, colors[s][0], colors[s][1], colors[s][2]});
        vertices.insert(vertices.end(), {-halfSize, halfSize, 0.0f, colors[s][0], colors[s][1], colors[s][2]});
        vertices.insert(vertices.end(), {halfSize, halfSize, 0.0f, colors[s][0], colors[s][1], colors[s][2]});
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

void createEllipse()
{
    std::vector<float> vertices;
    int segments = 50;
    float centerX = 0.0f, centerY = 0.0f;
    float radiusX = 0.2f, radiusY = 0.12f; // Scaled y to 60%
    
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * M_PI * i / segments;
        float x = centerX + radiusX * cos(angle);
        float y = centerY + radiusY * sin(angle);
        
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
        
        // Red color for ellipse
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
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

void createCircle(float radius = 0.1f)
{
    std::vector<float> vertices;
    int segments = 50;
    float centerX = 0.0f, centerY = 0.0f;
    
    // Center vertex
    vertices.insert(vertices.end(), {centerX, centerY, 0.0f, window2Color[0], window2Color[1], window2Color[2]});
    
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * M_PI * i / segments;
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        
        vertices.insert(vertices.end(), {x, y, 0.0f, window2Color[0], window2Color[1], window2Color[2]});
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

void createTriangle()
{
    std::vector<float> vertices;
    float size = 0.15f;
    
    // Triangle vertices
    vertices.insert(vertices.end(), {0.0f, size, 0.0f, window2Color[0], window2Color[1], window2Color[2]});      // top
    vertices.insert(vertices.end(), {-size, -size, 0.0f, window2Color[0], window2Color[1], window2Color[2]});   // bottom left
    vertices.insert(vertices.end(), {size, -size, 0.0f, window2Color[0], window2Color[1], window2Color[2]});    // bottom right
    
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

unsigned int createBreathingCircle(float x, float y, float r, float g, float b)
{
    std::vector<float> vertices;
    int segments = 30;
    float radius = 0.05f;
    
    // Center vertex
    vertices.insert(vertices.end(), {x, y, 0.0f, r, g, b});
    
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * M_PI * i / segments;
        float cx = x + radius * cos(angle);
        float cy = y + radius * sin(angle);
        
        vertices.insert(vertices.end(), {cx, cy, 0.0f, r, g, b});
    }
    
    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    return vao;
}

void displayMain()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shaderProgram);
    
    // Draw nested squares with rotation
    glBindVertexArray(squaresVAO);
    for (int i = 0; i < 4; i++)
    {
        if (animationEnabled)
        {
            // Apply rotation to squares (counter-clockwise)
            glPushMatrix();
            glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);
        }
        glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
        if (animationEnabled)
        {
            glPopMatrix();
        }
    }
    
    glBindVertexArray(0);
    glutSwapBuffers();
}

void displaySub()
{
    // Set subwindow background color
    glClearColor(subWindowBgColor[0], subWindowBgColor[1], subWindowBgColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shaderProgram);
    
    // Draw ellipse
    glBindVertexArray(ellipseVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 51); // 50 segments + 1 for closure
    glBindVertexArray(0);
    
    glutSwapBuffers();
}

void displayWindow2()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shaderProgram);
    
    // Draw circle (breathing effect)
    glBindVertexArray(circleVAO);
    if (animationEnabled)
    {
        glPushMatrix();
        glScalef(breathingScale, breathingScale, 1.0f);
    }
    glDrawArrays(GL_TRIANGLE_FAN, 0, 52); // 50 segments + center + 1 for closure
    if (animationEnabled)
    {
        glPopMatrix();
    }
    
    // Draw triangle with rotation
    glBindVertexArray(triangleVAO);
    if (animationEnabled)
    {
        glPushMatrix();
        glRotatef(-rotationAngle, 0.0f, 0.0f, 1.0f); // Clockwise rotation
    }
    glDrawArrays(GL_TRIANGLES, 0, 3);
    if (animationEnabled)
    {
        glPopMatrix();
    }
    
    glBindVertexArray(0);
    glutSwapBuffers();
}

void idle()
{
    if (animationEnabled)
    {
        rotationAngle += 1.0f;
        if (rotationAngle >= 360.0f) rotationAngle -= 360.0f;
        
        // Breathing effect
        breathingScale += breathingDirection * 0.02f;
        if (breathingScale >= 1.5f)
        {
            breathingScale = 1.5f;
            breathingDirection = -1.0f;
        }
        else if (breathingScale <= 0.5f)
        {
            breathingScale = 0.5f;
            breathingDirection = 1.0f;
        }
    }
    
    glutPostRedisplay();
}

void keyboardWindow2(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'r': case 'R': window2Color[0] = 1.0f; window2Color[1] = 0.0f; window2Color[2] = 0.0f; break; // Red
        case 'g': case 'G': window2Color[0] = 0.0f; window2Color[1] = 1.0f; window2Color[2] = 0.0f; break; // Green
        case 'b': case 'B': window2Color[0] = 0.0f; window2Color[1] = 0.0f; window2Color[2] = 1.0f; break; // Blue
        case 'y': case 'Y': window2Color[0] = 1.0f; window2Color[1] = 1.0f; window2Color[2] = 0.0f; break; // Yellow
        case 'o': case 'O': window2Color[0] = 1.0f; window2Color[1] = 0.5f; window2Color[2] = 0.0f; break; // Orange
        case 'p': case 'P': window2Color[0] = 1.0f; window2Color[1] = 0.0f; window2Color[2] = 1.0f; break; // Purple
        case 'w': case 'W': window2Color[0] = 1.0f; window2Color[1] = 1.0f; window2Color[2] = 1.0f; break; // White
    }
    
    // Update circle and triangle colors
    createCircle();
    createTriangle();
}

void mouseMain(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // Convert screen coordinates to normalized coordinates
        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
        
        float normalizedX = (2.0f * x / windowWidth) - 1.0f;
        float normalizedY = 1.0f - (2.0f * y / windowHeight);
        
        // Generate random color
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        float r = dis(gen);
        float g = dis(gen);
        float b = dis(gen);
        
        // Create breathing circle at click position
        unsigned int vao = createBreathingCircle(normalizedX, normalizedY, r, g, b);
        breathingCirclesVAO.push_back(vao);
        breathingCirclesData.insert(breathingCirclesData.end(), {normalizedX, normalizedY, r, g, b});
    }
}

void menuSub(int value)
{
    switch (value)
    {
        case 1: subWindowBgColor[0] = 1.0f; subWindowBgColor[1] = 0.0f; subWindowBgColor[2] = 0.0f; break; // Red
        case 2: subWindowBgColor[0] = 0.0f; subWindowBgColor[1] = 1.0f; subWindowBgColor[2] = 0.0f; break; // Green
        case 3: subWindowBgColor[0] = 0.0f; subWindowBgColor[1] = 0.0f; subWindowBgColor[2] = 1.0f; break; // Blue
        case 4: subWindowBgColor[0] = 1.0f; subWindowBgColor[1] = 1.0f; subWindowBgColor[2] = 0.0f; break; // Yellow
        case 5: subWindowBgColor[0] = 1.0f; subWindowBgColor[1] = 0.0f; subWindowBgColor[2] = 1.0f; break; // Purple
        case 6: subWindowBgColor[0] = 0.0f; subWindowBgColor[1] = 1.0f; subWindowBgColor[2] = 1.0f; break; // Cyan
    }
}

void menuMain(int value)
{
    switch (value)
    {
        case 1: animationEnabled = false; break; // Stop Animation
        case 2: animationEnabled = true; break;  // Start Animation
        case 3: // Square Colors - White
            // Reset squares to white/black
            createNestedSquares();
            break;
        case 4: // Square Colors - Red
            // Change squares to red gradient
            createNestedSquares();
            break;
        case 5: // Square Colors - Green
            // Change squares to green gradient
            createNestedSquares();
            break;
    }
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
    createNestedSquares();
    createEllipse();
    createCircle();
    createTriangle();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Enable double buffering
    
    // Create main window
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 100);
    mainWindow = glutCreateWindow("Main Window - Black & White Squares");
    
    init();
    
    glutDisplayFunc(displayMain);
    glutIdleFunc(idle);
    glutMouseFunc(mouseMain);
    
    // Create main window menu (right-click)
    int mainMenu = glutCreateMenu(menuMain);
    glutAddMenuEntry("Stop Animation", 1);
    glutAddMenuEntry("Start Animation", 2);
    
    int colorSubMenu = glutCreateMenu(menuMain);
    glutAddMenuEntry("White", 3);
    glutAddMenuEntry("Red", 4);
    glutAddMenuEntry("Green", 5);
    glutAddSubMenu("Square Colors", colorSubMenu);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    // Create subwindow
    subWindow = glutCreateSubWindow(mainWindow, 50, 50, 150, 150);
    glutDisplayFunc(displaySub);
    glutIdleFunc(idle);
    
    // Create subwindow menu
    int subMenu = glutCreateMenu(menuSub);
    glutAddMenuEntry("Red Background", 1);
    glutAddMenuEntry("Green Background", 2);
    glutAddMenuEntry("Blue Background", 3);
    glutAddMenuEntry("Yellow Background", 4);
    glutAddMenuEntry("Purple Background", 5);
    glutAddMenuEntry("Cyan Background", 6);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    // Create window 2
    glutInitWindowPosition(600, 100);
    window2 = glutCreateWindow("Window 2 - Circle & Triangle");
    glutDisplayFunc(displayWindow2);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboardWindow2);
    
    glutMainLoop();
    
    return 0;
}
