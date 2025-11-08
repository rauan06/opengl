#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>

// Global variables
unsigned int shaderProgram;
unsigned int pickingShaderProgram;
unsigned int objectVAOs[3], objectVBOs[3], objectEBOs[3];
unsigned int fbo, colorTexture, depthRenderbuffer;

// Object colors (diffuse)
float objectColors[3][3] = {
    {1.0f, 0.2f, 0.2f},  // Object 0: Red
    {0.2f, 1.0f, 0.2f},  // Object 1: Green
    {0.2f, 0.2f, 1.0f}   // Object 2: Blue
};

// Picking colors (unique constant colors for FBO)
float pickingColors[3][3] = {
    {1.0f, 0.0f, 0.0f},  // Object 0: Pure Red
    {0.0f, 1.0f, 0.0f},  // Object 1: Pure Green
    {0.0f, 0.0f, 1.0f}   // Object 2: Pure Blue
};

// Camera parameters
float cameraAngleX = 30.0f;
float cameraAngleY = 45.0f;
float cameraDistance = 8.0f;
float cameraTargetX = 0.0f;
float cameraTargetY = 0.0f;
float cameraTargetZ = 0.0f;

// Light position (in camera space)
float lightPos[3] = {0.0f, 0.0f, 5.0f};

// Material properties
float ka = 0.3f;
float kd = 0.7f;
float ks = 0.5f;
float shininess = 32.0f;

// Anti-aliasing
bool antiAliasingEnabled = false;
int windowWidth = 800;
int windowHeight = 600;

// Shader source code (Phong shading)
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

// Picking shader (constant color, no lighting)
const char* pickingVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* pickingFragmentShaderSource = R"(
#version 330 core
uniform vec3 pickingColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(pickingColor, 1.0);
}
)";

// Create a simple mesh (cube-like shape)
void createMesh(int objectIndex, float size, float offsetX, float offsetY, float offsetZ) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    // Create a box-like mesh
    float s = size;
    float positions[][3] = {
        // Front face
        {-s, -s,  s}, { s, -s,  s}, { s,  s,  s}, {-s,  s,  s},
        // Back face
        {-s, -s, -s}, {-s,  s, -s}, { s,  s, -s}, { s, -s, -s},
        // Top face
        {-s,  s, -s}, {-s,  s,  s}, { s,  s,  s}, { s,  s, -s},
        // Bottom face
        {-s, -s, -s}, { s, -s, -s}, { s, -s,  s}, {-s, -s,  s},
        // Right face
        { s, -s, -s}, { s,  s, -s}, { s,  s,  s}, { s, -s,  s},
        // Left face
        {-s, -s, -s}, {-s, -s,  s}, {-s,  s,  s}, {-s,  s, -s}
    };
    
    // Normals for each face
    float normals[][3] = {
        {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1},  // Front
        {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1},  // Back
        {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0},  // Top
        {0, -1, 0}, {0, -1, 0}, {0, -1, 0}, {0, -1, 0},  // Bottom
        {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0},  // Right
        {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}   // Left
    };
    
    // Add vertices with normals
    for (int i = 0; i < 24; i++) {
        vertices.push_back(positions[i][0] + offsetX);
        vertices.push_back(positions[i][1] + offsetY);
        vertices.push_back(positions[i][2] + offsetZ);
        vertices.push_back(normals[i][0]);
        vertices.push_back(normals[i][1]);
        vertices.push_back(normals[i][2]);
    }
    
    // Create indices for triangles
    unsigned int baseIndices[] = {
        0, 1, 2,  2, 3, 0,   // Front
        4, 5, 6,  6, 7, 4,   // Back
        8, 9, 10, 10, 11, 8, // Top
        12, 13, 14, 14, 15, 12, // Bottom
        16, 17, 18, 18, 19, 16, // Right
        20, 21, 22, 22, 23, 20  // Left
    };
    
    for (int i = 0; i < 36; i++) {
        indices.push_back(baseIndices[i]);
    }
    
    // Create VAO, VBO, EBO
    glGenVertexArrays(1, &objectVAOs[objectIndex]);
    glGenBuffers(1, &objectVBOs[objectIndex]);
    glGenBuffers(1, &objectEBOs[objectIndex]);
    
    glBindVertexArray(objectVAOs[objectIndex]);
    
    glBindBuffer(GL_ARRAY_BUFFER, objectVBOs[objectIndex]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objectEBOs[objectIndex]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

// Create FBO for picking
void createFBO() {
    // Create FBO
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    // Create texture for color attachment
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    
    // Create renderbuffer for depth
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    
    // Check FBO completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Resize FBO
void resizeFBO(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    
    // Resize texture
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    
    // Resize renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
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

// Render scene to FBO for picking
void renderPickingScene() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Calculate camera position
    float camX = cameraTargetX + cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    float camY = cameraTargetY + cameraDistance * sin(cameraAngleX * M_PI / 180.0f);
    float camZ = cameraTargetZ + cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    
    float model[16], view[16], projection[16];
    createIdentityMatrix(model);
    createLookAtMatrix(view, camX, camY, camZ, cameraTargetX, cameraTargetY, cameraTargetZ, 0.0f, 1.0f, 0.0f);
    createPerspectiveMatrix(projection, 45.0f, (float)windowWidth / windowHeight, 0.1f, 100.0f);
    
    glUseProgram(pickingShaderProgram);
    
    glUniformMatrix4fv(glGetUniformLocation(pickingShaderProgram, "model"), 1, GL_FALSE, model);
    glUniformMatrix4fv(glGetUniformLocation(pickingShaderProgram, "view"), 1, GL_FALSE, view);
    glUniformMatrix4fv(glGetUniformLocation(pickingShaderProgram, "projection"), 1, GL_FALSE, projection);
    
    // Draw objects with picking colors
    float offsets[3][3] = {
        {-2.0f, 0.0f, 0.0f},  // Object 0
        { 0.0f, 0.0f, 0.0f},  // Object 1
        { 2.0f, 0.0f, 0.0f}   // Object 2
    };
    
    for (int i = 0; i < 3; i++) {
        // Create model matrix with translation
        createIdentityMatrix(model);
        model[12] = offsets[i][0];
        model[13] = offsets[i][1];
        model[14] = offsets[i][2];
        
        glUniformMatrix4fv(glGetUniformLocation(pickingShaderProgram, "model"), 1, GL_FALSE, model);
        glUniform3f(glGetUniformLocation(pickingShaderProgram, "pickingColor"), 
                   pickingColors[i][0], pickingColors[i][1], pickingColors[i][2]);
        
        glBindVertexArray(objectVAOs[i]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Get object ID from pixel color
int getObjectIdFromPixel(int x, int y) {
    renderPickingScene();
    
    // Read pixel from FBO
    unsigned char pixel[3];
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(x, windowHeight - y - 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    
    // Convert pixel color to object ID
    float r = pixel[0] / 255.0f;
    float g = pixel[1] / 255.0f;
    float b = pixel[2] / 255.0f;
    
    // Check which picking color matches (with tolerance)
    for (int i = 0; i < 3; i++) {
        if (fabs(r - pickingColors[i][0]) < 0.1f &&
            fabs(g - pickingColors[i][1]) < 0.1f &&
            fabs(b - pickingColors[i][2]) < 0.1f) {
            return i;
        }
    }
    
    return -1; // No object found
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int objectId = getObjectIdFromPixel(x, y);
        if (objectId >= 0 && objectId < 3) {
            // Change object color to random value
            objectColors[objectId][0] = (float)rand() / RAND_MAX;
            objectColors[objectId][1] = (float)rand() / RAND_MAX;
            objectColors[objectId][2] = (float)rand() / RAND_MAX;
            std::cout << "Object " << objectId << " clicked! New color: ("
                     << objectColors[objectId][0] << ", "
                     << objectColors[objectId][1] << ", "
                     << objectColors[objectId][2] << ")" << std::endl;
            glutPostRedisplay();
        }
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'a':
        case 'A':
            antiAliasingEnabled = !antiAliasingEnabled;
            if (antiAliasingEnabled) {
                glEnable(GL_MULTISAMPLE);
                std::cout << "Anti-aliasing enabled" << std::endl;
            } else {
                glDisable(GL_MULTISAMPLE);
                std::cout << "Anti-aliasing disabled" << std::endl;
            }
            break;
        case 'r':
        case 'R':
            cameraAngleX = 30.0f;
            cameraAngleY = 45.0f;
            cameraDistance = 8.0f;
            break;
        case 27: // ESC
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
    glViewport(0, 0, windowWidth, windowHeight);
    
    // Calculate camera position
    float camX = cameraTargetX + cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    float camY = cameraTargetY + cameraDistance * sin(cameraAngleX * M_PI / 180.0f);
    float camZ = cameraTargetZ + cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f);
    
    float model[16], view[16], projection[16];
    createIdentityMatrix(model);
    createLookAtMatrix(view, camX, camY, camZ, cameraTargetX, cameraTargetY, cameraTargetZ, 0.0f, 1.0f, 0.0f);
    createPerspectiveMatrix(projection, 45.0f, (float)windowWidth / windowHeight, 0.1f, 100.0f);
    
    glUseProgram(shaderProgram);
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, projection);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos[0], lightPos[1], lightPos[2]);
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), camX, camY, camZ);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "ka"), ka);
    glUniform1f(glGetUniformLocation(shaderProgram, "kd"), kd);
    glUniform1f(glGetUniformLocation(shaderProgram, "ks"), ks);
    glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), shininess);
    
    // Draw objects
    float offsets[3][3] = {
        {-2.0f, 0.0f, 0.0f},  // Object 0
        { 0.0f, 0.0f, 0.0f},  // Object 1
        { 2.0f, 0.0f, 0.0f}   // Object 2
    };
    
    for (int i = 0; i < 3; i++) {
        createIdentityMatrix(model);
        model[12] = offsets[i][0];
        model[13] = offsets[i][1];
        model[14] = offsets[i][2];
        
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, model);
        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 
                   objectColors[i][0], objectColors[i][1], objectColors[i][2]);
        
        glBindVertexArray(objectVAOs[i]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    
    glBindVertexArray(0);
    
    glutSwapBuffers();
}

void reshape(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
    resizeFBO(width, height);
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
    
    // Compile main shaders
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Compile picking shaders
    unsigned int pickingVertexShader = compileShader(GL_VERTEX_SHADER, pickingVertexShaderSource);
    unsigned int pickingFragmentShader = compileShader(GL_FRAGMENT_SHADER, pickingFragmentShaderSource);
    
    pickingShaderProgram = glCreateProgram();
    glAttachShader(pickingShaderProgram, pickingVertexShader);
    glAttachShader(pickingShaderProgram, pickingFragmentShader);
    glLinkProgram(pickingShaderProgram);
    
    glDeleteShader(pickingVertexShader);
    glDeleteShader(pickingFragmentShader);
    
    // Create meshes
    createMesh(0, 0.8f, -2.0f, 0.0f, 0.0f);
    createMesh(1, 0.8f, 0.0f, 0.0f, 0.0f);
    createMesh(2, 0.8f, 2.0f, 0.0f, 0.0f);
    
    // Create FBO
    createFBO();
    
    // Initialize random seed
    srand(time(nullptr));
    
    std::cout << "\n=== Anti-aliasing and Picking ===" << std::endl;
    std::cout << "Left click: Select object (changes color)" << std::endl;
    std::cout << "A/a: Toggle anti-aliasing" << std::endl;
    std::cout << "Arrow keys: Rotate camera" << std::endl;
    std::cout << "R/r: Reset view" << std::endl;
    std::cout << "ESC: Exit" << std::endl;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Assignment 4 Part 2 - Anti-aliasing and Picking");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    
    glutMainLoop();
    
    return 0;
}

