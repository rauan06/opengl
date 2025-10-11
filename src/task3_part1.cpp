#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>

// Global variables
int mainWindow, subWindow, window2;
bool animationEnabled = true;
float squareRotation = 0.0f;
float triangleRotation = 0.0f;
float circleScale = 1.0f;
float circleScaleDirection = 0.01f;
float currentColor[3] = {1.0f, 1.0f, 1.0f}; // Default white
float subWindowBgColor[3] = {0.2f, 0.2f, 0.2f}; // Default dark gray
std::vector<std::pair<float, float>> breathingCircles; // For extra credit

// Color definitions
float colors[][3] = {
    {1.0f, 0.0f, 0.0f}, // Red
    {0.0f, 1.0f, 0.0f}, // Green
    {0.0f, 0.0f, 1.0f}, // Blue
    {1.0f, 1.0f, 0.0f}, // Yellow
    {1.0f, 0.5f, 0.0f}, // Orange
    {0.5f, 0.0f, 1.0f}, // Purple
    {1.0f, 1.0f, 1.0f}  // White
};

// Function prototypes
void drawSquare(float x, float y, float size, float r, float g, float b);
void drawEllipse(float x, float y, float radiusX, float radiusY, float r, float g, float b);
void drawCircle(float x, float y, float radius, float r, float g, float b);
void drawTriangle(float x, float y, float size, float r, float g, float b);
void drawBreathingCircle(float x, float y, float baseRadius, float r, float g, float b);

// Main window display function
void mainDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw black and white squares
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    if (animationEnabled) {
        glRotatef(squareRotation, 0.0f, 0.0f, 1.0f);
    }
    
    // Draw nested squares (black and white pattern)
    float sizes[] = {0.8f, 0.6f, 0.4f, 0.2f};
    for (int i = 0; i < 4; i++) {
        if (i % 2 == 0) {
            drawSquare(0.0f, 0.0f, sizes[i], 1.0f, 1.0f, 1.0f); // White
        } else {
            drawSquare(0.0f, 0.0f, sizes[i], 0.0f, 0.0f, 0.0f); // Black
        }
    }
    glPopMatrix();
    
    glutSwapBuffers();
}

// Sub window display function
void subDisplay() {
    glClearColor(subWindowBgColor[0], subWindowBgColor[1], subWindowBgColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw ellipse
    drawEllipse(0.0f, 0.0f, 0.3f, 0.2f, 1.0f, 0.0f, 0.0f);
    
    glutSwapBuffers();
}

// Window 2 display function
void window2Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw circle
    glPushMatrix();
    glTranslatef(-0.3f, 0.0f, 0.0f);
    if (animationEnabled) {
        glScalef(circleScale, circleScale, 1.0f);
    }
    drawCircle(0.0f, 0.0f, 0.2f, currentColor[0], currentColor[1], currentColor[2]);
    glPopMatrix();
    
    // Draw triangle
    glPushMatrix();
    glTranslatef(0.3f, 0.0f, 0.0f);
    if (animationEnabled) {
        glRotatef(triangleRotation, 0.0f, 0.0f, 1.0f);
    }
    drawTriangle(0.0f, 0.0f, 0.3f, currentColor[0], currentColor[1], currentColor[2]);
    glPopMatrix();
    
    glutSwapBuffers();
}

// Idle function for animation
void idle() {
    if (animationEnabled) {
        // Rotate squares counter-clockwise
        squareRotation += 1.0f;
        if (squareRotation >= 360.0f) squareRotation -= 360.0f;
        
        // Rotate triangle clockwise
        triangleRotation -= 1.5f;
        if (triangleRotation <= -360.0f) triangleRotation += 360.0f;
        
        // Breathing circle animation
        circleScale += circleScaleDirection;
        if (circleScale >= 1.5f || circleScale <= 0.5f) {
            circleScaleDirection = -circleScaleDirection;
        }
        
        // Update breathing circles for extra credit
        // Each circle has its own breathing animation
        // This is a simplified version - in a full implementation,
        // each circle would have its own scale factor
    }
    
    glutPostRedisplay();
}

// Keyboard function for window 2
void window2Keyboard(unsigned char key, int /*x*/, int /*y*/) {
    switch (key) {
        case 'r': case 'R':
            currentColor[0] = colors[0][0]; currentColor[1] = colors[0][1]; currentColor[2] = colors[0][2];
            break;
        case 'g': case 'G':
            currentColor[0] = colors[1][0]; currentColor[1] = colors[1][1]; currentColor[2] = colors[1][2];
            break;
        case 'b': case 'B':
            currentColor[0] = colors[2][0]; currentColor[1] = colors[2][1]; currentColor[2] = colors[2][2];
            break;
        case 'y': case 'Y':
            currentColor[0] = colors[3][0]; currentColor[1] = colors[3][1]; currentColor[2] = colors[3][2];
            break;
        case 'o': case 'O':
            currentColor[0] = colors[4][0]; currentColor[1] = colors[4][1]; currentColor[2] = colors[4][2];
            break;
        case 'p': case 'P':
            currentColor[0] = colors[5][0]; currentColor[1] = colors[5][1]; currentColor[2] = colors[5][2];
            break;
        case 'w': case 'W':
            currentColor[0] = colors[6][0]; currentColor[1] = colors[6][1]; currentColor[2] = colors[6][2];
            break;
    }
    glutPostRedisplay();
}

// Mouse function for main window (extra credit)
void mainMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Convert screen coordinates to world coordinates
        float worldX = (float)(x - 250) / 250.0f;
        float worldY = (float)(250 - y) / 250.0f;
        
        // Add a new breathing circle at the click position
        breathingCircles.push_back(std::make_pair(worldX, worldY));
    }
}

// Menu functions
void subMenu(int value) {
    switch (value) {
        case 1: // Red background
            subWindowBgColor[0] = 1.0f; subWindowBgColor[1] = 0.0f; subWindowBgColor[2] = 0.0f;
            break;
        case 2: // Green background
            subWindowBgColor[0] = 0.0f; subWindowBgColor[1] = 1.0f; subWindowBgColor[2] = 0.0f;
            break;
        case 3: // Blue background
            subWindowBgColor[0] = 0.0f; subWindowBgColor[1] = 0.0f; subWindowBgColor[2] = 1.0f;
            break;
        case 4: // Yellow background
            subWindowBgColor[0] = 1.0f; subWindowBgColor[1] = 1.0f; subWindowBgColor[2] = 0.0f;
            break;
    }
    glutPostRedisplay();
}

void mainMenu(int value) {
    switch (value) {
        case 1: // Stop Animation
            animationEnabled = false;
            break;
        case 2: // Start Animation
            animationEnabled = true;
            break;
        case 3: // White squares
            // This would change square colors - simplified for this example
            break;
        case 4: // Red squares
            // This would change square colors - simplified for this example
            break;
        case 5: // Green squares
            // This would change square colors - simplified for this example
            break;
    }
}

// Drawing functions
void drawSquare(float x, float y, float size, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x - size/2, y - size/2);
    glVertex2f(x + size/2, y - size/2);
    glVertex2f(x + size/2, y + size/2);
    glVertex2f(x - size/2, y + size/2);
    glEnd();
}

void drawEllipse(float x, float y, float radiusX, float radiusY, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center
    for (int i = 0; i <= 50; i++) {
        float angle = 2.0f * M_PI * i / 50.0f;
        glVertex2f(x + radiusX * cos(angle), y + radiusY * sin(angle));
    }
    glEnd();
}

void drawCircle(float x, float y, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center
    for (int i = 0; i <= 50; i++) {
        float angle = 2.0f * M_PI * i / 50.0f;
        glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
    }
    glEnd();
}

void drawTriangle(float x, float y, float size, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + size); // Top
    glVertex2f(x - size, y - size); // Bottom left
    glVertex2f(x + size, y - size); // Bottom right
    glEnd();
}

void drawBreathingCircle(float x, float y, float baseRadius, float r, float g, float b) {
    float breathingRadius = baseRadius * circleScale;
    drawCircle(x, y, breathingRadius, r, g, b);
}

// Reshape functions
void mainReshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void subReshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void window2Reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    srand(time(NULL));
    
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Enable double buffering
    
    // Create main window
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    mainWindow = glutCreateWindow("Main Window - Black & White Squares");
    
    // Set up main window callbacks
    glutDisplayFunc(mainDisplay);
    glutReshapeFunc(mainReshape);
    glutIdleFunc(idle);
    glutMouseFunc(mainMouse);
    
    // Create main window menu
    glutCreateMenu(mainMenu);
    glutAddMenuEntry("Stop Animation", 1);
    glutAddMenuEntry("Start Animation", 2);
    glutAddMenuEntry("Square Colors ->", 0);
    glutAddMenuEntry("  White", 3);
    glutAddMenuEntry("  Red", 4);
    glutAddMenuEntry("  Green", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    // Create sub window
    glutInitWindowSize(300, 200);
    glutInitWindowPosition(650, 100);
    subWindow = glutCreateWindow("Sub Window - Ellipse");
    
    // Set up sub window callbacks
    glutDisplayFunc(subDisplay);
    glutReshapeFunc(subReshape);
    
    // Create sub window menu
    glutCreateMenu(subMenu);
    glutAddMenuEntry("Red Background", 1);
    glutAddMenuEntry("Green Background", 2);
    glutAddMenuEntry("Blue Background", 3);
    glutAddMenuEntry("Yellow Background", 4);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    // Create window 2
    glutInitWindowSize(400, 300);
    glutInitWindowPosition(100, 650);
    window2 = glutCreateWindow("Window 2 - Circle & Triangle (r,g,b,y,o,p,w)");
    
    // Set up window 2 callbacks
    glutDisplayFunc(window2Display);
    glutReshapeFunc(window2Reshape);
    glutKeyboardFunc(window2Keyboard);
    
    // Set clear color for main window
    glutSetWindow(mainWindow);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    
    // Set clear color for window 2
    glutSetWindow(window2);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    // Start the main loop
    glutMainLoop();
    
    return 0;
}
