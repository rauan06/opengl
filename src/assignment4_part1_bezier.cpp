#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>

// Global variables
unsigned int shaderProgram;
unsigned int patchVAO, patchVBO, patchEBO;
unsigned int controlPointsVAO, controlPointsVBO;
unsigned int axesVAO, axesVBO;

// Control points (4x4 grid, 16 points total)
// Format: X-Y ordering [0,0], [1,0], [2,0], [3,0], [0,1], [1,1], [2,1], [3,1], etc.
std::vector<float> controlPoints = {
    // Row 0 (u=0)
    -1.5f, -1.5f,  0.0f,  // [0,0]
     0.5f, -1.5f,  0.0f,  // [1,0]
     0.5f, -1.5f,  0.0f,  // [2,0]
     1.5f, -1.5f,  0.0f,  // [3,0]
    // Row 1 (u=1/3)
    -1.5f, -0.5f,  0.5f,  // [0,1]
     0.5f, -0.5f,  1.0f,  // [1,1]
     0.5f, -0.5f,  1.0f,  // [2,1]
     1.5f, -0.5f,  0.5f,  // [3,1]
    // Row 2 (u=2/3)
    -1.5f,  0.5f,  0.5f,  // [0,2]
     0.5f,  0.5f,  1.0f,  // [1,2]
     0.5f,  0.5f,  1.0f,  // [2,2]
     1.5f,  0.5f,  0.5f,  // [3,2]
    // Row 3 (u=1)
    -1.5f,  1.5f,  0.0f,  // [0,3]
     0.5f,  1.5f,  0.0f,  // [1,3]
     0.5f,  1.5f,  0.0f,  // [2,3]
     1.5f,  1.5f,  0.0f   // [3,3]
};

// Tessellation resolution
int resolution = 10;

// Selected control point (0-15)
int selectedPoint = 0;

// Camera parameters
float cameraAngleX = 30.0f;
float cameraAngleY = 45.0f;
float cameraDistance = 5.0f;
float cameraTargetX = 0.0f;
float cameraTargetY = 0.0f;
float cameraTargetZ = 0.0f;

// Light position (in camera space, near camera)
float lightPos[3] = {0.0f, 0.0f, 2.0f};

// Material properties for Phong shading
float ka = 0.3f;  // Ambient coefficient
float kd = 0.7f;  // Diffuse coefficient
float ks = 0.5f;  // Specular coefficient
float shininess = 32.0f;

// Shader source code
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightDir;
out vec3 ViewDir;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Transform light and view positions to world space
    vec3 worldLightPos = lightPos;
    vec3 worldViewPos = viewPos;
    
    LightDir = normalize(worldLightPos - FragPos);
    ViewDir = normalize(worldViewPos - FragPos);
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 LightDir;
in vec3 ViewDir;

uniform float ka;
uniform float kd;
uniform float ks;
uniform float shininess;
uniform vec3 lightColor;
uniform vec3 objectColor;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightDir);
    vec3 viewDir = normalize(ViewDir);
    
    // Ambient component
    vec3 ambient = ka * lightColor * objectColor;
    
    // Diffuse component
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = kd * diff * lightColor * objectColor;
    
    // Specular component
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = ks * spec * lightColor;
    
    // Combine components
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
)";

// Bezier basis functions
float bezierBasis(int i, float t) {
    float oneMinusT = 1.0f - t;
    switch(i) {
        case 0: return oneMinusT * oneMinusT * oneMinusT;
        case 1: return 3.0f * oneMinusT * oneMinusT * t;
        case 2: return 3.0f * oneMinusT * t * t;
        case 3: return t * t * t;
        default: return 0.0f;
    }
}

// Evaluate bicubic Bezier patch at (u, v)
void evaluateBezierPatch(float u, float v, float* result) {
    result[0] = 0.0f;
    result[1] = 0.0f;
    result[2] = 0.0f;
    
    // Clamp u and v to [0, 1]
    u = std::max(0.0f, std::min(1.0f, u));
    v = std::max(0.0f, std::min(1.0f, v));
    
    // Iterative evaluation (not recursive)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int idx = i * 4 + j;
            float basis = bezierBasis(i, u) * bezierBasis(j, v);
            result[0] += controlPoints[idx * 3 + 0] * basis;
            result[1] += controlPoints[idx * 3 + 1] * basis;
            result[2] += controlPoints[idx * 3 + 2] * basis;
        }
    }
}

// Compute partial derivative with respect to u
void evaluateBezierPatchDu(float u, float v, float* result) {
    result[0] = 0.0f;
    result[1] = 0.0f;
    result[2] = 0.0f;
    
    u = std::max(0.0f, std::min(1.0f, u));
    v = std::max(0.0f, std::min(1.0f, v));
    
    // Derivative of Bezier basis functions
    float dBasis[4];
    float oneMinusU = 1.0f - u;
    dBasis[0] = -3.0f * oneMinusU * oneMinusU;
    dBasis[1] = 3.0f * (oneMinusU * oneMinusU - 2.0f * oneMinusU * u);
    dBasis[2] = 3.0f * (2.0f * oneMinusU * u - u * u);
    dBasis[3] = 3.0f * u * u;
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int idx = i * 4 + j;
            float basis = dBasis[i] * bezierBasis(j, v);
            result[0] += controlPoints[idx * 3 + 0] * basis;
            result[1] += controlPoints[idx * 3 + 1] * basis;
            result[2] += controlPoints[idx * 3 + 2] * basis;
        }
    }
}

// Compute partial derivative with respect to v
void evaluateBezierPatchDv(float u, float v, float* result) {
    result[0] = 0.0f;
    result[1] = 0.0f;
    result[2] = 0.0f;
    
    u = std::max(0.0f, std::min(1.0f, u));
    v = std::max(0.0f, std::min(1.0f, v));
    
    // Derivative of Bezier basis functions
    float dBasis[4];
    float oneMinusV = 1.0f - v;
    dBasis[0] = -3.0f * oneMinusV * oneMinusV;
    dBasis[1] = 3.0f * (oneMinusV * oneMinusV - 2.0f * oneMinusV * v);
    dBasis[2] = 3.0f * (2.0f * oneMinusV * v - v * v);
    dBasis[3] = 3.0f * v * v;
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int idx = i * 4 + j;
            float basis = bezierBasis(i, u) * dBasis[j];
            result[0] += controlPoints[idx * 3 + 0] * basis;
            result[1] += controlPoints[idx * 3 + 1] * basis;
            result[2] += controlPoints[idx * 3 + 2] * basis;
        }
    }
}

// Compute surface normal using cross product of partial derivatives
void computeNormal(float u, float v, float* normal) {
    float du[3], dv[3];
    evaluateBezierPatchDu(u, v, du);
    evaluateBezierPatchDv(u, v, dv);
    
    // Cross product: normal = du × dv
    normal[0] = du[1] * dv[2] - du[2] * dv[1];
    normal[1] = du[2] * dv[0] - du[0] * dv[2];
    normal[2] = du[0] * dv[1] - du[1] * dv[0];
    
    // Normalize
    float len = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    if (len > 1e-6f) {
        normal[0] /= len;
        normal[1] /= len;
        normal[2] /= len;
    }
}

// Generate patch mesh
void generatePatchMesh() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    int numVertices = (resolution + 1) * (resolution + 1);
    
    // Generate vertices
    for (int j = 0; j <= resolution; j++) {
        float v = (float)j / resolution;
        for (int i = 0; i <= resolution; i++) {
            float u = (float)i / resolution;
            
            // Position
            float pos[3];
            evaluateBezierPatch(u, v, pos);
            vertices.push_back(pos[0]);
            vertices.push_back(pos[1]);
            vertices.push_back(pos[2]);
            
            // Normal
            float normal[3];
            computeNormal(u, v, normal);
            vertices.push_back(normal[0]);
            vertices.push_back(normal[1]);
            vertices.push_back(normal[2]);
        }
    }
    
    // Generate indices for triangles
    for (int j = 0; j < resolution; j++) {
        for (int i = 0; i < resolution; i++) {
            int topLeft = j * (resolution + 1) + i;
            int topRight = topLeft + 1;
            int bottomLeft = (j + 1) * (resolution + 1) + i;
            int bottomRight = bottomLeft + 1;
            
            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
    // Update VAO, VBO, EBO
    if (patchVAO == 0) {
        glGenVertexArrays(1, &patchVAO);
        glGenBuffers(1, &patchVBO);
        glGenBuffers(1, &patchEBO);
    }
    
    glBindVertexArray(patchVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, patchVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, patchEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

// Generate control points for rendering
void generateControlPoints() {
    std::vector<float> vertices;
    
    for (size_t i = 0; i < controlPoints.size(); i += 3) {
        vertices.push_back(controlPoints[i]);
        vertices.push_back(controlPoints[i + 1]);
        vertices.push_back(controlPoints[i + 2]);
    }
    
    if (controlPointsVAO == 0) {
        glGenVertexArrays(1, &controlPointsVAO);
        glGenBuffers(1, &controlPointsVBO);
    }
    
    glBindVertexArray(controlPointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, controlPointsVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

// Generate axes (positions only, colors set in shader)
void generateAxes() {
    float vertices[] = {
        // X axis
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // origin with dummy normal
        0.5f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // X end with dummy normal
        // Y axis
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // origin
        0.0f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  // Y end
        // Z axis
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // origin
        0.0f, 0.0f, 0.5f,  0.0f, 0.0f, 1.0f   // Z end
    };
    
    if (axesVAO == 0) {
        glGenVertexArrays(1, &axesVAO);
        glGenBuffers(1, &axesVBO);
    }
    
    glBindVertexArray(axesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

// Matrix functions
void createIdentityMatrix(float* matrix) {
    for (int i = 0; i < 16; i++) {
        matrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }
}

void createLookAtMatrix(float* matrix, float eyeX, float eyeY, float eyeZ,
                       float centerX, float centerY, float centerZ,
                       float upX, float upY, float upZ) {
    float f[3] = {centerX - eyeX, centerY - eyeY, centerZ - eyeZ};
    float len = sqrt(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
    if (len > 1e-6f) {
        f[0] /= len; f[1] /= len; f[2] /= len;
    }
    
    float s[3] = {
        f[1] * upZ - f[2] * upY,
        f[2] * upX - f[0] * upZ,
        f[0] * upY - f[1] * upX
    };
    len = sqrt(s[0]*s[0] + s[1]*s[1] + s[2]*s[2]);
    if (len > 1e-6f) {
        s[0] /= len; s[1] /= len; s[2] /= len;
    }
    
    float u[3] = {
        s[1] * f[2] - s[2] * f[1],
        s[2] * f[0] - s[0] * f[2],
        s[0] * f[1] - s[1] * f[0]
    };
    
    createIdentityMatrix(matrix);
    matrix[0] = s[0]; matrix[4] = s[1]; matrix[8] = s[2];
    matrix[1] = u[0]; matrix[5] = u[1]; matrix[9] = u[2];
    matrix[2] = -f[0]; matrix[6] = -f[1]; matrix[10] = -f[2];
    matrix[12] = -(s[0]*eyeX + s[1]*eyeY + s[2]*eyeZ);
    matrix[13] = -(u[0]*eyeX + u[1]*eyeY + u[2]*eyeZ);
    matrix[14] = f[0]*eyeX + f[1]*eyeY + f[2]*eyeZ;
}

void createPerspectiveMatrix(float* matrix, float fovy, float aspect, float near, float far) {
    float f = 1.0f / tan(fovy * M_PI / 360.0f);
    
    createIdentityMatrix(matrix);
    matrix[0] = f / aspect;
    matrix[5] = f;
    matrix[10] = (far + near) / (near - far);
    matrix[11] = -1.0f;
    matrix[14] = (2.0f * far * near) / (near - far);
    matrix[15] = 0.0f;
}

void printInstructions() {
    std::cout << "\n=== Bezier Patch - Interactive Control ===" << std::endl;
    std::cout << "\nCONTROL POINT SELECTION:" << std::endl;
    std::cout << "  0-9, A-F: Select control point (0-15)" << std::endl;
    std::cout << "\nCONTROL POINT MODIFICATION:" << std::endl;
    std::cout << "  X/x: Decrease/Increase X position" << std::endl;
    std::cout << "  Y/y: Decrease/Increase Y position" << std::endl;
    std::cout << "  Z/z: Decrease/Increase Z position" << std::endl;
    std::cout << "\nTESSELLATION:" << std::endl;
    std::cout << "  +/-: Decrease/Increase resolution" << std::endl;
    std::cout << "\nCAMERA CONTROLS:" << std::endl;
    std::cout << "  Arrow keys: Rotate camera" << std::endl;
    std::cout << "  R/r: Reset view" << std::endl;
    std::cout << "\nOTHER:" << std::endl;
    std::cout << "  ESC: Exit" << std::endl;
    std::cout << "\nCurrent selected point: " << selectedPoint << std::endl;
    std::cout << "Current resolution: " << resolution << "x" << resolution << std::endl;
}

void keyboard(unsigned char key, int x, int y) {
    float delta = 0.1f;
    
    switch(key) {
        // Control point selection
        case '0': selectedPoint = 0; break;
        case '1': selectedPoint = 1; break;
        case '2': selectedPoint = 2; break;
        case '3': selectedPoint = 3; break;
        case '4': selectedPoint = 4; break;
        case '5': selectedPoint = 5; break;
        case '6': selectedPoint = 6; break;
        case '7': selectedPoint = 7; break;
        case '8': selectedPoint = 8; break;
        case '9': selectedPoint = 9; break;
        case 'a':
        case 'A': selectedPoint = 10; break;
        case 'b':
        case 'B': selectedPoint = 11; break;
        case 'c':
        case 'C': selectedPoint = 12; break;
        case 'd':
        case 'D': selectedPoint = 13; break;
        case 'e':
        case 'E': selectedPoint = 14; break;
        case 'f':
        case 'F': selectedPoint = 15; break;
        
        // Control point modification
        case 'x':
            controlPoints[selectedPoint * 3 + 0] += delta;
            generatePatchMesh();
            generateControlPoints();
            break;
        case 'X':
            controlPoints[selectedPoint * 3 + 0] -= delta;
            generatePatchMesh();
            generateControlPoints();
            break;
        case 'y':
            controlPoints[selectedPoint * 3 + 1] += delta;
            generatePatchMesh();
            generateControlPoints();
            break;
        case 'Y':
            controlPoints[selectedPoint * 3 + 1] -= delta;
            generatePatchMesh();
            generateControlPoints();
            break;
        case 'z':
            controlPoints[selectedPoint * 3 + 2] += delta;
            generatePatchMesh();
            generateControlPoints();
            break;
        case 'Z':
            controlPoints[selectedPoint * 3 + 2] -= delta;
            generatePatchMesh();
            generateControlPoints();
            break;
        
        // Tessellation
        case '+':
        case '=':
            resolution = std::min(50, resolution + 1);
            generatePatchMesh();
            std::cout << "Resolution: " << resolution << "x" << resolution << std::endl;
            break;
        case '-':
        case '_':
            resolution = std::max(3, resolution - 1);
            generatePatchMesh();
            std::cout << "Resolution: " << resolution << "x" << resolution << std::endl;
            break;
        
        // Reset view
        case 'r':
        case 'R':
            cameraAngleX = 30.0f;
            cameraAngleY = 45.0f;
            cameraDistance = 5.0f;
            break;
        
        // Exit
        case 27: // ESC
            exit(0);
            break;
    }
    
    if ((key >= '0' && key <= '9') || (key >= 'a' && key <= 'f') || (key >= 'A' && key <= 'F')) {
        std::cout << "Selected control point: " << selectedPoint << std::endl;
    }
    
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    float delta = 5.0f;
    
    switch(key) {
        case GLUT_KEY_UP:
            cameraAngleX += delta;
            break;
        case GLUT_KEY_DOWN:
            cameraAngleX -= delta;
            break;
        case GLUT_KEY_LEFT:
            cameraAngleY -= delta;
            break;
        case GLUT_KEY_RIGHT:
            cameraAngleY += delta;
            break;
    }
    
    glutPostRedisplay();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Calculate camera position
    float camX = cameraTargetX + cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    float camY = cameraTargetY + cameraDistance * sin(cameraAngleX * M_PI / 180.0f);
    float camZ = cameraTargetZ + cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    
    // Matrices
    float model[16], view[16], projection[16];
    createIdentityMatrix(model);
    createLookAtMatrix(view, camX, camY, camZ, cameraTargetX, cameraTargetY, cameraTargetZ, 0.0f, 1.0f, 0.0f);
    createPerspectiveMatrix(projection, 45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    
    glUseProgram(shaderProgram);
    
    // Set matrices
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, model);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, projection);
    
    // Set light and material properties
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos[0], lightPos[1], lightPos[2]);
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), camX, camY, camZ);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.2f, 0.6f, 0.8f);
    glUniform1f(glGetUniformLocation(shaderProgram, "ka"), ka);
    glUniform1f(glGetUniformLocation(shaderProgram, "kd"), kd);
    glUniform1f(glGetUniformLocation(shaderProgram, "ks"), ks);
    glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), shininess);
    
    // Draw patch
    glBindVertexArray(patchVAO);
    int numIndices = resolution * resolution * 6;
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    
    // Draw control points with simple rendering
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(controlPointsVAO);
    glPointSize(10.0f);
    for (int i = 0; i < 16; i++) {
        if (i == selectedPoint) {
            glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 1.0f, 0.0f); // Yellow for selected
            glPointSize(12.0f);
        } else {
            glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 0.0f, 0.0f); // Red for others
            glPointSize(8.0f);
        }
        glDrawArrays(GL_POINTS, i, 1);
    }
    glEnable(GL_DEPTH_TEST);
    
    // Draw axes (X=red, Y=green, Z=blue) with flat shading
    glBindVertexArray(axesVAO);
    glLineWidth(3.0f);
    // X axis (red)
    glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 0.0f, 0.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "ka"), 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "kd"), 0.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "ks"), 0.0f);
    glDrawArrays(GL_LINES, 0, 2);
    // Y axis (green)
    glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_LINES, 2, 2);
    // Z axis (blue)
    glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 4, 2);
    // Restore lighting
    glUniform1f(glGetUniformLocation(shaderProgram, "ka"), ka);
    glUniform1f(glGetUniformLocation(shaderProgram, "kd"), kd);
    glUniform1f(glGetUniformLocation(shaderProgram, "ks"), ks);
    
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
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
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
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Generate geometry
    generatePatchMesh();
    generateControlPoints();
    generateAxes();
    
    printInstructions();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Assignment 4 Part 1 - Bezier Patch");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    
    glutMainLoop();
    
    return 0;
}

