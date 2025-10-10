#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <vector>

// Transformation types
enum TransformationType {
    SCALE = 0,
    ROTATE = 1,
    TRANSLATE = 2
};

// Global variables
unsigned int shaderProgram;
unsigned int cubeVAO, cubeVBO;
TransformationType currentTransformation = SCALE;

// Transformation values
float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;
float rotateX = 0.0f, rotateY = 0.0f, rotateZ = 0.0f;
float translateX = 0.0f, translateY = 0.0f, translateZ = 0.0f;

// Transformation deltas
float scaleDelta = 0.1f;
float rotateDelta = 10.0f; // degrees
float translateDelta = 0.1f;

// Transformation matrices
float scaleMatrix[16];
float rotateXMatrix[16], rotateYMatrix[16], rotateZMatrix[16];
float translateMatrix[16];
float finalMatrix[16];

// Shader source code
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 transform;

out vec3 vertexColor;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
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

// Matrix operations
void createIdentityMatrix(float* matrix) {
    for (int i = 0; i < 16; i++) {
        matrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }
}

void createScaleMatrix(float sx, float sy, float sz, float* matrix) {
    createIdentityMatrix(matrix);
    matrix[0] = sx;   // X scale
    matrix[5] = sy;   // Y scale
    matrix[10] = sz;  // Z scale
}

void createRotationXMatrix(float angle, float* matrix) {
    createIdentityMatrix(matrix);
    float c = cos(angle * M_PI / 180.0f);
    float s = sin(angle * M_PI / 180.0f);
    matrix[5] = c;
    matrix[6] = -s;
    matrix[9] = s;
    matrix[10] = c;
}

void createRotationYMatrix(float angle, float* matrix) {
    createIdentityMatrix(matrix);
    float c = cos(angle * M_PI / 180.0f);
    float s = sin(angle * M_PI / 180.0f);
    matrix[0] = c;
    matrix[2] = s;
    matrix[8] = -s;
    matrix[10] = c;
}

void createRotationZMatrix(float angle, float* matrix) {
    createIdentityMatrix(matrix);
    float c = cos(angle * M_PI / 180.0f);
    float s = sin(angle * M_PI / 180.0f);
    matrix[0] = c;
    matrix[1] = -s;
    matrix[4] = s;
    matrix[5] = c;
}

void createTranslationMatrix(float tx, float ty, float tz, float* matrix) {
    createIdentityMatrix(matrix);
    matrix[12] = tx;  // X translation
    matrix[13] = ty;  // Y translation
    matrix[14] = tz;  // Z translation
}

void multiplyMatrices(const float* a, const float* b, float* result) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i * 4 + j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                result[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
            }
        }
    }
}

void updateTransformationMatrix() {
    // Create individual transformation matrices
    createScaleMatrix(scaleX, scaleY, scaleZ, scaleMatrix);
    createRotationXMatrix(rotateX, rotateXMatrix);
    createRotationYMatrix(rotateY, rotateYMatrix);
    createRotationZMatrix(rotateZ, rotateZMatrix);
    createTranslationMatrix(translateX, translateY, translateZ, translateMatrix);
    
    // Apply transformations in order: Scale -> Rotation -> Translation
    float tempMatrix[16];
    
    // First: Scale
    createIdentityMatrix(finalMatrix);
    multiplyMatrices(finalMatrix, scaleMatrix, tempMatrix);
    
    // Second: Rotation (X, Y, Z)
    multiplyMatrices(tempMatrix, rotateXMatrix, finalMatrix);
    multiplyMatrices(finalMatrix, rotateYMatrix, tempMatrix);
    multiplyMatrices(tempMatrix, rotateZMatrix, finalMatrix);
    
    // Third: Translation
    multiplyMatrices(finalMatrix, translateMatrix, tempMatrix);
    
    // Copy final result
    for (int i = 0; i < 16; i++) {
        finalMatrix[i] = tempMatrix[i];
    }
}

void createCube() {
    // Cube vertices with RGB colors as specified in lecture
    // Each face has vertices with different colors for gradient effect
    std::vector<float> vertices = {
        // Front face (Z = 0.5)
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // Bottom-left: Red
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // Bottom-right: Green
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // Top-right: Blue
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // Top-right: Blue
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, // Top-left: Yellow
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // Bottom-left: Red
        
        // Back face (Z = -0.5)
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, // Bottom-left: Magenta
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // Top-right: Cyan
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, // Bottom-right: Green
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // Top-right: Cyan
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, // Bottom-left: Magenta
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, // Top-left: Yellow
        
        // Left face (X = -0.5)
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, // Top-front: Yellow
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, // Top-back: Yellow
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, // Bottom-back: Magenta
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, // Bottom-back: Magenta
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // Bottom-front: Red
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, // Top-front: Yellow
        
        // Right face (X = 0.5)
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // Top-front: Blue
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, // Bottom-back: Green
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // Top-back: Cyan
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, // Bottom-back: Green
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // Top-front: Blue
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // Bottom-front: Green
        
        // Top face (Y = 0.5)
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // Front-right: Blue
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, // Back-left: Yellow
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // Back-right: Cyan
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, // Back-left: Yellow
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // Front-right: Blue
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, // Front-left: Yellow
        
        // Bottom face (Y = -0.5)
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // Front-right: Green
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, // Back-right: Green
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, // Back-left: Magenta
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, // Back-left: Magenta
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, // Front-left: Red
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f  // Front-right: Green
    };
    
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void printInstructions() {
    std::cout << "\n=== 3D Colored Cube - Interactive Transformations ===" << std::endl;
    std::cout << "\nTRANSFORMATION SELECTION:" << std::endl;
    std::cout << "  'S' - Select SCALE transformation" << std::endl;
    std::cout << "  'R' - Select ROTATE transformation" << std::endl;
    std::cout << "  'T' - Select TRANSLATE transformation" << std::endl;
    
    std::cout << "\nTRANSFORMATION MODIFICATION (X, Y, Z):" << std::endl;
    std::cout << "  'Q'/'A' - Decrease/Increase X component" << std::endl;
    std::cout << "  'W'/'Z' - Decrease/Increase Y component" << std::endl;
    std::cout << "  'E'/'D' - Decrease/Increase Z component" << std::endl;
    
    std::cout << "\nDELTA MODIFICATION:" << std::endl;
    std::cout << "  '-'/'+' - Decrease/Increase transformation delta" << std::endl;
    
    std::cout << "\nOTHER CONTROLS:" << std::endl;
    std::cout << "  '0' - Reset all transformations to default" << std::endl;
    std::cout << "  ESC - Exit program" << std::endl;
    
    std::cout << "\nCurrent transformation: ";
    switch(currentTransformation) {
        case SCALE: std::cout << "SCALE"; break;
        case ROTATE: std::cout << "ROTATE"; break;
        case TRANSLATE: std::cout << "TRANSLATE"; break;
    }
    std::cout << std::endl;
}

void keyboard(unsigned char key, [[maybe_unused]] int x, [[maybe_unused]] int y) {
    switch (key) {
        // Transformation selection
        case 's':
        case 'S':
            currentTransformation = SCALE;
            std::cout << "Selected: SCALE transformation" << std::endl;
            break;
        case 'r':
        case 'R':
            currentTransformation = ROTATE;
            std::cout << "Selected: ROTATE transformation" << std::endl;
            break;
        case 't':
        case 'T':
            currentTransformation = TRANSLATE;
            std::cout << "Selected: TRANSLATE transformation" << std::endl;
            break;
            
        // X component controls
        case 'a':
        case 'A':
            if (currentTransformation == SCALE) scaleX -= scaleDelta;
            else if (currentTransformation == ROTATE) rotateX -= rotateDelta;
            else if (currentTransformation == TRANSLATE) translateX -= translateDelta;
            updateTransformationMatrix();
            break;
        case 'q':
        case 'Q':
            if (currentTransformation == SCALE) scaleX += scaleDelta;
            else if (currentTransformation == ROTATE) rotateX += rotateDelta;
            else if (currentTransformation == TRANSLATE) translateX += translateDelta;
            updateTransformationMatrix();
            break;
            
        // Y component controls
        case 'w':
        case 'W':
            if (currentTransformation == SCALE) scaleY -= scaleDelta;
            else if (currentTransformation == ROTATE) rotateY -= rotateDelta;
            else if (currentTransformation == TRANSLATE) translateY -= translateDelta;
            updateTransformationMatrix();
            break;
        case 'z':
        case 'Z':
            if (currentTransformation == SCALE) scaleY += scaleDelta;
            else if (currentTransformation == ROTATE) rotateY += rotateDelta;
            else if (currentTransformation == TRANSLATE) translateY += translateDelta;
            updateTransformationMatrix();
            break;
            
        // Z component controls
        case 'e':
        case 'E':
            if (currentTransformation == SCALE) scaleZ -= scaleDelta;
            else if (currentTransformation == ROTATE) rotateZ -= rotateDelta;
            else if (currentTransformation == TRANSLATE) translateZ -= translateDelta;
            updateTransformationMatrix();
            break;
        case 'd':
        case 'D':
            if (currentTransformation == SCALE) scaleZ += scaleDelta;
            else if (currentTransformation == ROTATE) rotateZ += rotateDelta;
            else if (currentTransformation == TRANSLATE) translateZ += translateDelta;
            updateTransformationMatrix();
            break;
            
        // Delta modification
        case '-':
            if (currentTransformation == SCALE) scaleDelta *= 0.9f;
            else if (currentTransformation == ROTATE) rotateDelta *= 0.9f;
            else if (currentTransformation == TRANSLATE) translateDelta *= 0.9f;
            break;
        case '+':
        case '=':
            if (currentTransformation == SCALE) scaleDelta *= 1.1f;
            else if (currentTransformation == ROTATE) rotateDelta *= 1.1f;
            else if (currentTransformation == TRANSLATE) translateDelta *= 1.1f;
            break;
            
        // Reset
        case '0':
            scaleX = scaleY = scaleZ = 1.0f;
            rotateX = rotateY = rotateZ = 0.0f;
            translateX = translateY = translateZ = 0.0f;
            updateTransformationMatrix();
            std::cout << "All transformations reset to default" << std::endl;
            break;
            
        // Exit
        case 27: // ESC key
            exit(0);
            break;
    }
    
    glutPostRedisplay();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    
    // Set transformation matrix uniform
    int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, finalMatrix);
    
    // Draw cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
}

unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    
    return shader;
}

void init() {
    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
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
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
    }
    
    // Delete shaders as they're now linked
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Create cube
    createCube();
    
    // Initialize transformation matrix
    updateTransformationMatrix();
    
    // Print instructions
    printInstructions();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Task 3 - 3D Colored Cube with Transformations");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    
    return 0;
}
