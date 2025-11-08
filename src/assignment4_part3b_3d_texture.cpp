#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>

// Global variables
unsigned int shaderProgram;
unsigned int meshVAO, meshVBO, meshEBO;
int meshIndexCount = 0;

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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 FragPos;
out vec3 Normal;
out vec3 WorldPos;
out vec3 LightDir;
out vec3 ViewDir;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    WorldPos = FragPos;  // World position for 3D texturing
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
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
in vec3 WorldPos;
in vec3 LightDir;
in vec3 ViewDir;

uniform float ka;
uniform float kd;
uniform float ks;
uniform float shininess;
uniform vec3 lightColor;

out vec4 FragColor;

// 3D Procedural Texture Function
// Creates an interesting 3D pattern based on world coordinates
vec3 compute3DTexture(vec3 pos) {
    // Scale factor for texture frequency
    float scale = 2.0;
    
    // Create multiple wave patterns in 3D space
    float x = pos.x * scale;
    float y = pos.y * scale;
    float z = pos.z * scale;
    
    // Spherical distance from origin
    float radius = length(pos);
    
    // Create interesting 3D patterns
    // Pattern 1: Radial waves
    float radialWave = sin(radius * 3.0) * 0.5 + 0.5;
    
    // Pattern 2: Spiral pattern along Z-axis
    float angle = atan(pos.y, pos.x);
    float spiral = sin(angle * 3.0 + radius * 2.0) * 0.5 + 0.5;
    
    // Pattern 3: 3D noise-like pattern using multiple sine waves
    float noiseX = sin(x * 2.0) * cos(y * 2.0) * sin(z * 2.0);
    float noiseY = cos(x * 2.5) * sin(y * 2.5) * cos(z * 2.5);
    float noiseZ = sin(x * 3.0) * sin(y * 3.0) * cos(z * 3.0);
    
    // Pattern 4: Layered waves in different directions
    float waveXY = sin(x + y) * 0.5 + 0.5;
    float waveXZ = sin(x + z) * 0.5 + 0.5;
    float waveYZ = sin(y + z) * 0.5 + 0.5;
    
    // Combine patterns to create interesting texture
    float r = radialWave * 0.3 + spiral * 0.2 + (noiseX * 0.5 + 0.5) * 0.3 + waveXY * 0.2;
    float g = spiral * 0.3 + (noiseY * 0.5 + 0.5) * 0.4 + waveXZ * 0.3;
    float b = radialWave * 0.2 + (noiseZ * 0.5 + 0.5) * 0.4 + waveYZ * 0.4;
    
    // Add some color variation based on position
    r = mix(r, abs(sin(x * 0.5)), 0.2);
    g = mix(g, abs(cos(y * 0.5)), 0.2);
    b = mix(b, abs(sin(z * 0.5)), 0.2);
    
    // Normalize to [0, 1] range
    r = clamp(r, 0.0, 1.0);
    g = clamp(g, 0.0, 1.0);
    b = clamp(b, 0.0, 1.0);
    
    return vec3(r, g, b);
}

void main()
{
    // Compute 3D texture color from world coordinates
    vec3 textureColor = compute3DTexture(WorldPos);
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightDir);
    vec3 viewDir = normalize(ViewDir);
    
    // Ambient component
    vec3 ambient = ka * lightColor * textureColor;
    
    // Diffuse component (3D texture replaces constant diffuse color)
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

// Create a torus mesh
void createTorusMesh(float majorRadius, float minorRadius, int majorSegments, int minorSegments) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    // Generate vertices
    for (int i = 0; i <= majorSegments; i++) {
        float u = (float)i / majorSegments * 2.0f * M_PI;
        float cosU = cos(u);
        float sinU = sin(u);
        
        for (int j = 0; j <= minorSegments; j++) {
            float v = (float)j / minorSegments * 2.0f * M_PI;
            float cosV = cos(v);
            float sinV = sin(v);
            
            // Position
            float x = (majorRadius + minorRadius * cosV) * cosU;
            float y = (majorRadius + minorRadius * cosV) * sinU;
            float z = minorRadius * sinV;
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // Normal
            float nx = cosV * cosU;
            float ny = cosV * sinU;
            float nz = sinV;
            
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
    }
    
    // Generate indices
    for (int i = 0; i < majorSegments; i++) {
        for (int j = 0; j < minorSegments; j++) {
            int first = i * (minorSegments + 1) + j;
            int second = first + minorSegments + 1;
            
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);
            
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &meshVBO);
    glGenBuffers(1, &meshEBO);
    
    glBindVertexArray(meshVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    meshIndexCount = indices.size();
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

// Alternative: Create a sphere mesh
void createSphereMesh(float radius, int segments) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    // Generate vertices
    for (int i = 0; i <= segments; i++) {
        float theta = (float)i / segments * M_PI;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);
        
        for (int j = 0; j <= segments; j++) {
            float phi = (float)j / segments * 2.0f * M_PI;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);
            
            // Position
            float x = radius * sinTheta * cosPhi;
            float y = radius * sinTheta * sinPhi;
            float z = radius * cosTheta;
            
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // Normal (same as position for unit sphere, normalized)
            vertices.push_back(x / radius);
            vertices.push_back(y / radius);
            vertices.push_back(z / radius);
        }
    }
    
    // Generate indices
    for (int i = 0; i < segments; i++) {
        for (int j = 0; j < segments; j++) {
            int first = i * (segments + 1) + j;
            int second = first + segments + 1;
            
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);
            
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &meshVBO);
    glGenBuffers(1, &meshEBO);
    
    glBindVertexArray(meshVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    meshIndexCount = indices.size();
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
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
    
    glBindVertexArray(meshVAO);
    glDrawElements(GL_TRIANGLES, meshIndexCount, GL_UNSIGNED_INT, 0);
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
    
    // Create a torus mesh (can be replaced with SMF loader)
    createTorusMesh(1.5f, 0.5f, 32, 16);
    // Alternative: createSphereMesh(1.5f, 32);
    
    std::cout << "Assignment 4 Part 3b - 3D Procedural Texturing" << std::endl;
    std::cout << "Arrow keys: Rotate camera" << std::endl;
    std::cout << "R/r: Reset view" << std::endl;
    std::cout << "ESC: Exit" << std::endl;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Assignment 4 Part 3b - 3D Procedural Texturing");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    
    glutMainLoop();
    
    return 0;
}

