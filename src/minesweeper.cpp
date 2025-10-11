#include <bits/stdc++.h>
#include <GLFW/glfw3.h>
using namespace std;

// ---------- Constants ----------
const long long bombVal      = -9876543219LL;
const long long safeVal      = -9000000000LL;
const long long openedOffset = 9000000000LL;

// ---------- Minesweeper Logic ----------
void SetBombCounts(vector<vector<long long>> &grid) {
    int H = (int)grid.size();
    if (H == 0) return;
    int W = (int)grid[0].size();
    const vector<pair<int,int>> directions = {
        {-1,-1},{-1,0},{-1,1},
        {0,-1},       {0,1},
        {1,-1},{1,0},{1,1}
    };
    for (int i=0;i<H;++i){
        for (int j=0;j<W;++j){
            if (grid[i][j] == bombVal){
                for (auto &d: directions){
                    int x = i + d.first;
                    int y = j + d.second;
                    if (x>=0 && x<H && y>=0 && y<W && grid[x][y] != bombVal){
                        grid[x][y]++;
                    }
                }
            }
        }
    }
}

bool IsWinCondition(const vector<vector<long long>> &grid) {
    for (const auto &row : grid) {
        for (auto cell : row) {
            if (cell == bombVal) continue;
            else if (cell < 0) return false;
        }
    }
    return true;
}

void OpenCell(int x, int y, int gridHeight, int gridWidth, vector<vector<long long>> &grid) {
    if (x < 0 || x >= gridWidth || y < 0 || y >= gridHeight || grid[y][x] >= 0) return;

    if (grid[y][x] != safeVal) {
        grid[y][x] += openedOffset;
        return;
    }

    grid[y][x] += openedOffset;

    static const vector<pair<int,int>> directions = {
        {-1, 1}, {-1, 0}, {-1, -1},
        {0, 1}, {0, -1},
        {1, 1}, {1, 0}, {1, -1}
    };

    for (auto &d : directions) {
        OpenCell(x + d.first, y + d.second, gridHeight, gridWidth, grid);
    }
}

int RunMove(int x, int y, int grid_height, int grid_width, vector<vector<long long>> &grid) {
    if (x<0||x>=grid_width||y<0||y>=grid_height) return 0;
    if (grid[y][x] == bombVal) return -1;
    OpenCell(x, y, grid_height, grid_width, grid);
    if (IsWinCondition(grid)) return 1;
    return 0;
}

void OpenAllBombCells(const vector<pair<int,int>> &bombCoords, vector<vector<long long>> &grid) {
    for (const auto &b : bombCoords) {
        int y = b.first, x = b.second;
        if (y >= 0 && y < (int)grid.size() && x >= 0 && x < (int)grid[0].size()) {
            grid[y][x] = -grid[y][x];
        }
    }
}

vector<vector<long long>> GenerateRandomMap(int height, int width, vector<pair<int,int>> &bombCoords) {
    vector<vector<long long>> grid(height, vector<long long>(width, safeVal));
    int bombCount = (height * width) / 6;
    if (bombCount < 2) bombCount = 2;
    while (bombCount > 0) {
        int x = rand() % width;
        int y = rand() % height;
        if (grid[y][x] != bombVal) {
            grid[y][x] = bombVal;
            bombCoords.emplace_back(y, x);
            bombCount--;
        }
    }
    return grid;
}

// ---------- OpenGL Application ----------
struct GLApp {
    GLFWwindow* window = nullptr;
    
    // Grid dimensions
    int rows = 10;
    int cols = 10;
    
    // Visual constants (never change)
    const float cellSize = 36.0f;
    const float padding = 2.0f;
    const int marginX = 50;
    const int marginY = 50;
    
    // Game state
    vector<vector<long long>> grid;
    vector<pair<int,int>> bombs;
    vector<vector<char>> flags;
    bool gameOver = false;
    bool win = false;

    // Calculate required window size based on grid
    void calculateWindowSize(int &outW, int &outH) const {
        float cellFull = cellSize + padding * 2;
        outW = int(cols * cellFull + marginX * 2);
        outH = int(rows * cellFull + marginY * 2);
    }

    void resetGame() {
        bombs.clear();
        grid = GenerateRandomMap(rows, cols, bombs);
        SetBombCounts(grid);
        flags.assign(rows, vector<char>(cols, 0));
        gameOver = false;
        win = false;
    }

    bool init(int r, int c) {
        rows = r;
        cols = c;
        
        if (!glfwInit()) {
            cerr << "Failed to init GLFW\n";
            return false;
        }
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

        int winW, winH;
        calculateWindowSize(winW, winH);
        
        window = glfwCreateWindow(winW, winH, "Minesweeper OpenGL", NULL, NULL);
        if (!window) {
            cerr << "Failed to create window\n";
            glfwTerminate();
            return false;
        }
        
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);

        // Framebuffer callback
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* w, int ww, int hh){
            glViewport(0, 0, ww, hh);
        });

        // Mouse callback
        glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods){
            if (action != GLFW_PRESS) return;
            GLApp* app = (GLApp*)glfwGetWindowUserPointer(w);
            if (!app) return;
            app->onMouseClick(button);
        });

        resetGame();
        return true;
    }

    void onMouseClick(int button) {
        if (gameOver) {
            resetGame();
            return;
        }

        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        
        int winW, winH;
        glfwGetWindowSize(window, &winW, &winH);
        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        
        if (winW == 0 || winH == 0) return;
        
        // Convert to framebuffer coordinates
        double scaleX = double(fbW) / double(winW);
        double scaleY = double(fbH) / double(winH);
        int sx = int(mx * scaleX);
        int sy = int(my * scaleY);

        // Convert to cell coordinates
        int cx, cy;
        if (!screenToCell(sx, sy, cx, cy)) return;

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            int res = RunMove(cx, cy, rows, cols, grid);
            if (res == -1) {
                gameOver = true;
                OpenAllBombCells(bombs, grid);
            } else if (res == 1) {
                win = true;
                gameOver = true;
            }
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (grid[cy][cx] < 0) {
                flags[cy][cx] = !flags[cy][cx];
            }
        }
    }

    bool screenToCell(int sx, int sy, int &cx, int &cy) const {
        float cellFull = cellSize + padding * 2;
        float fx = (sx - marginX) / cellFull;
        float fy = (sy - marginY) / cellFull;
        
        if (fx < 0 || fy < 0) return false;
        
        cx = int(floor(fx));
        cy = int(floor(fy));
        
        return (cx >= 0 && cx < cols && cy >= 0 && cy < rows);
    }

    void drawRect(float x, float y, float w, float h, float r, float g, float b) {
        glColor3f(r, g, b);
        glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x+w, y);
            glVertex2f(x+w, y+h);
            glVertex2f(x, y+h);
        glEnd();
    }

    void drawBorder(float x, float y, float w, float h) {
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(x, y);
            glVertex2f(x+w, y);
            glVertex2f(x+w, y+h);
            glVertex2f(x, y+h);
        glEnd();
    }

    void drawNumber(float cx, float cy, int num) {
        if (num <= 0) return;
        
        float s = cellSize * 0.5f;
        float x = cx + (cellSize - s) / 2.0f;
        float y = cy + (cellSize - s) / 2.0f;
        
        float r=0, g=0, b=0;
        if (num==1 || num==7) { r=0.8f; g=0.4f; b=0.2f; }
        else if (num==2 || num==8) { g=0.6f; }
        else if (num==3) { r=0.9f; g=0.7f; }
        else if (num==4) { r=0.8f; }
        else if (num==6) { r=0.7f; g=0.3f; b=0.6f; }
        else { r=0.2f; g=0.5f; b=0.2f; }
        
        drawRect(x, y, s, s, r, g, b);
    }

    void drawFlag(float cx, float cy) {
        float s = cellSize * 0.5f;
        float x = cx + (cellSize - s) / 2;
        float y = cy + (cellSize - s) / 2;
        drawRect(x, y, s, s, 0.9f, 0.2f, 0.2f);
    }

    void drawBomb(float cx, float cy) {
        float s = cellSize * 0.5f;
        float x = cx + (cellSize - s) / 2;
        float y = cy + (cellSize - s) / 2;
        drawRect(x, y, s, s, 0.0f, 0.0f, 0.0f);
        drawRect(x+s*0.2f, y+s*0.2f, s*0.6f, s*0.6f, 0.9f, 0.1f, 0.1f);
    }

    void render() {
        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, fbW, fbH, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClearColor(0.92f, 0.92f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float cellFull = cellSize + padding * 2;

        // Board background
        float boardW = cols * cellFull;
        float boardH = rows * cellFull;
        drawRect(marginX - 8, marginY - 8, boardW + 16, boardH + 16, 0.8f, 0.8f, 0.85f);

        // Draw cells
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                float x = marginX + j * cellFull + padding;
                float y = marginY + i * cellFull + padding;
                long long cell = grid[i][j];

                if (cell < 0) {
                    // Hidden cell
                    drawRect(x, y, cellSize, cellSize, 0.55f, 0.65f, 0.75f);
                    drawBorder(x, y, cellSize, cellSize);
                    if (flags[i][j]) {
                        drawFlag(x, y);
                    }
                } else if (cell == -bombVal) {
                    // Revealed bomb
                    drawRect(x, y, cellSize, cellSize, 0.95f, 0.9f, 0.9f);
                    drawBomb(x, y);
                } else if (cell == 0) {
                    // Empty opened cell
                    drawRect(x, y, cellSize, cellSize, 0.98f, 0.98f, 0.98f);
                    drawBorder(x, y, cellSize, cellSize);
                } else {
                    // Numbered cell
                    drawRect(x, y, cellSize, cellSize, 0.98f, 0.98f, 0.98f);
                    drawBorder(x, y, cellSize, cellSize);
                    drawNumber(x, y, (int)cell);
                }
            }
        }

        // Status indicator
        if (gameOver) {
            if (win) {
                drawRect(10, 10, 150, 26, 0.95f, 0.92f, 0.4f);
            } else {
                drawRect(10, 10, 150, 26, 0.95f, 0.5f, 0.5f);
            }
        } else {
            drawRect(10, 10, 200, 26, 0.8f, 0.95f, 0.8f);
        }
    }

    void run() {
        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        glViewport(0, 0, fbW, fbH);

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            render();
            glfwSwapBuffers(window);
        }
        
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

// ---------- Main ----------
int main(int argc, char** argv) {
    srand((unsigned)time(nullptr));
    
    int rows = 10, cols = 14;
    if (argc >= 3) {
        rows = max(3, atoi(argv[1]));
        cols = max(3, atoi(argv[2]));
    }
    
    GLApp app;
    if (!app.init(rows, cols)) {
        return 1;
    }
    
    app.run();
    return 0;
}