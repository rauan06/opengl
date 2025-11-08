#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <vector>

// Global variables
unsigned int shaderProgram;
unsigned int patchVAO, patchVBO, patchEBO;
unsigned int textureID;

// Control points (4x4 grid)
std::vector<float> controlPoints = {
    // Row 0
    -1.5f, -1.5f,  0.0f,
     0.5f, -1.5f,  0.0f,
     0.5f, -1.5f,  0.0f,
     1.5f, -1.5f,  0.0f,
    // Row 1
    -1.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  1.0f,
     0.5f, -0.5f,  1.0f,
     1.5f, -0.5f,  0.5f,
    // Row 2
    -1.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  1.0f,
     0.5f,  0.5f,  1.0f,
     1.5f,  0.5f,  0.5f,
    // Row 3
    -1.5f,  1.5f,  0.0f,
     0.5f,  1.5f,  0.0f,
     0.5f,  1.5f,  0.0f,
     1.5f,  1.5f,  0.0f
};

int resolution = 12;

// Camera parameters
float cameraAngleX = 30.0f;
float cameraAngleY = 45.0f;
float cameraDistance = 5.0f;

// Light position
float lightPos[3] = {0.0f, 0.0f, 2.0f};

// Material properties
float ka = 0.3f;
float kd = 0.7f;
float ks = 0.5f;
float shininess = 32.0f;

// Shader source code
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec3 LightDir;
out vec3 ViewDir;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    
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
in vec2 TexCoord;
in vec3 LightDir;
in vec3 ViewDir;

uniform float ka;
uniform float kd;
uniform float ks;
uniform float shininess;
uniform vec3 lightColor;
uniform sampler2D textureSampler;

out vec4 FragColor;

void main()
{
    // Sample texture
    vec3 textureColor = texture(textureSampler, TexCoord).rgb;
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightDir);
    vec3 viewDir = normalize(ViewDir);
    
    // Ambient component
    vec3 ambient = ka * lightColor * textureColor;
    
    // Diffuse component (texture replaces constant diffuse color)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = kd * diff * lightColor * textureColor;
    
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

void evaluateBezierPatch(float u, float v, float* result) {
    result[0] = 0.0f;
    result[1] = 0.0f;
    result[2] = 0.0f;
    
    u = std::max(0.0f, std::min(1.0f, u));
    v = std::max(0.0f, std::min(1.0f, v));
    
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

void evaluateBezierPatchDu(float u, float v, float* result) {
    result[0] = 0.0f;
    result[1] = 0.0f;
    result[2] = 0.0f;
    
    u = std::max(0.0f, std::min(1.0f, u));
    v = std::max(0.0f, std::min(1.0f, v));
    
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

void evaluateBezierPatchDv(float u, float v, float* result) {
    result[0] = 0.0f;
    result[1] = 0.0f;
    result[2] = 0.0f;
    
    u = std::max(0.0f, std::min(1.0f, u));
    v = std::max(0.0f, std::min(1.0f, v));
    
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

void computeNormal(float u, float v, float* normal) {
    float du[3], dv[3];
    evaluateBezierPatchDu(u, v, du);
    evaluateBezierPatchDv(u, v, dv);
    
    normal[0] = du[1] * dv[2] - du[2] * dv[1];
    normal[1] = du[2] * dv[0] - du[0] * dv[2];
    normal[2] = du[0] * dv[1] - du[1] * dv[0];
    
    float len = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    if (len > 1e-6f) {
        normal[0] /= len;
        normal[1] /= len;
        normal[2] /= len;
    }
}

// Generate procedural texture (interesting pattern)
void generateProceduralTexture(int width, int height, unsigned char* data) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 3;
            
            // Create an interesting pattern (spiral/wave pattern)
            float fx = (float)x / width;
            float fy = (float)y / height;
            
            // Create spiral pattern
            float centerX = 0.5f;
            float centerY = 0.5f;
            float dx = fx - centerX;
            float dy = fy - centerY;
            float angle = atan2(dy, dx);
            float radius = sqrt(dx * dx + dy * dy);
            
            // Create wave patterns
            float wave1 = sin(radius * 10.0f + angle * 3.0f) * 0.5f + 0.5f;
            float wave2 = sin(fx * 8.0f * M_PI) * sin(fy * 8.0f * M_PI) * 0.5f + 0.5f;
            float wave3 = sin((fx + fy) * 6.0f * M_PI) * 0.5f + 0.5f;
            
            // Combine patterns
            float r = wave1 * 0.4f + wave2 * 0.3f + wave3 * 0.3f;
            float g = wave2 * 0.5f + wave3 * 0.5f;
            float b = wave1 * 0.6f + wave3 * 0.4f;
            
            data[idx + 0] = (unsigned char)(r * 255);
            data[idx + 1] = (unsigned char)(g * 255);
            data[idx + 2] = (unsigned char)(b * 255);
        }
    }
}

void createTexture() {
    const int width = 512;
    const int height = 512;
    unsigned char* data = new unsigned char[width * height * 3];
    
    generateProceduralTexture(width, height, data);
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    delete[] data;
}

void generatePatchMesh() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
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
            
            // Texture coordinates (same as u, v)
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }
    
    // Generate indices
    for (int j = 0; j < resolution; j++) {
        for (int i = 0; i < resolution; i++) {
            int topLeft = j * (resolution + 1) + i;
            int topRight = topLeft + 1;
            int bottomLeft = (j + 1) * (resolution + 1) + i;
            int bottomRight = bottomLeft + 1;
            
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
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
    
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
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

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'r':
        case 'R':
            cameraAngleX = 30.0f;
            cameraAngleY = 45.0f;
            cameraDistance = 5.0f;
            break;
        case 27:
            exit(0);
            break;
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
    
    float camX = cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    float camY = cameraDistance * sin(cameraAngleX * M_PI / 180.0f);
    float camZ = cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    
    float model[16], view[16], projection[16];
    createIdentityMatrix(model);
    createLookAtMatrix(view, camX, camY, camZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    createPerspectiveMatrix(projection, 45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    
    glUseProgram(shaderProgram);
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, model);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, projection);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos[0], lightPos[1], lightPos[2]);
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), camX, camY, camZ);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "ka"), ka);
    glUniform1f(glGetUniformLocation(shaderProgram, "kd"), kd);
    glUniform1f(glGetUniformLocation(shaderProgram, "ks"), ks);
    glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), shininess);
    
    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler"), 0);
    
    glBindVertexArray(patchVAO);
    int numIndices = resolution * resolution * 6;
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
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
    
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    createTexture();
    generatePatchMesh();
    
    std::cout << "Assignment 4 Part 3a - Texture Mapped Bezier Patch" << std::endl;
    std::cout << "Arrow keys: Rotate camera" << std::endl;
    std::cout << "R/r: Reset view" << std::endl;
    std::cout << "ESC: Exit" << std::endl;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Assignment 4 Part 3a - Texture Mapped Bezier Patch");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    
    glutMainLoop();
    
    return 0;
}

