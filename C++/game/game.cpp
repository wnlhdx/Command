#include <iostream>
#include <string>
#include <vector>
#include <conio.h> // 用于_getch()在Windows上捕获按键，需要MSVC编译器

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

const int COURT_WIDTH = 50;
const int COURT_HEIGHT = 15;
const int PADDLE_HEIGHT = 3;

int ball_x = COURT_WIDTH / 2;
int ball_y = COURT_HEIGHT / 2;
int ball_vx = 1;
int ball_vy = 1;

int paddle1_y = COURT_HEIGHT / 2 - PADDLE_HEIGHT;
int paddle2_y = COURT_HEIGHT / 2 - PADDLE_HEIGHT;

int gravity_x = COURT_WIDTH / 2; // 引力源位置
int gravity_strength = 1; // 引力强度
int gravity_y = COURT_HEIGHT / 2;

bool special_power_active = false; // 特殊技能激活标志


std::vector<std::string> buffer(COURT_HEIGHT, std::string(COURT_WIDTH, ' '));

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void drawCourt() {
    for (int y = 0; y < COURT_HEIGHT; ++y) {
        for (int x = 0; x < COURT_WIDTH; ++x) {
            if (x == 0 || x == COURT_WIDTH - 1) {
                buffer[y][x] = '|';
            }
            else if (y == ball_y && x == ball_x) {
                buffer[y][x] = 'O';
            }
            else if (x == 1) {
                if (y >= paddle1_y && y <= paddle1_y + PADDLE_HEIGHT-1) {
                    buffer[y][x] = '|';
                }
                else {
                    buffer[y][x] = ' ';
                }
            }
            else if (x == COURT_WIDTH - 2){
                if (y >= paddle2_y && y <= paddle2_y + PADDLE_HEIGHT-1) {
                    buffer[y][x] = '|';
                }
                else {
                    buffer[y][x] = ' ';
                }
            }
            else if (x == gravity_x && y == gravity_y) {
                buffer[y][x] = 'G';
            }
            else {
                buffer[y][x] = ' ';
            }
           
        }
    }
    
}

void render(int score1,int score2) {
    for (const auto& line : buffer) {
        std::cout << line << std::endl;
    }
    std::cout << "score1:" << score1 << std::endl;
    std::cout << "score2:" << score2 << std::endl;
}

void applyGravity() {
    // 计算球与引力源之间的距离
    int dx = gravity_x - ball_x;
    // 根据距离调整球的速度
    if (dx != 0) {
        ball_vx += gravity_strength * (dx > 0 ? -1 : 1);
    }
}

void gameLoop() {
    bool running = true;
    int score1 = 0;
    int score2 = 0;
    while (running) {
        drawCourt();
        clearScreen();
        render(score1, score2);
        // Move the ball
        ball_x += ball_vx;
        ball_y += ball_vy;

        if (special_power_active) {
            applyGravity();
        }

        // Collision detection for ball
        if (ball_x <= 1 && (ball_y >= paddle1_y && ball_y <= paddle1_y + PADDLE_HEIGHT - 1)) {
            ball_vx = -ball_vx;
        }
        else if (ball_x >= COURT_WIDTH - 3 && (ball_y >= paddle2_y && ball_y <= paddle2_y + PADDLE_HEIGHT - 1)) {
            ball_vx = -ball_vx;
        }
        if (ball_x <= 0 || ball_x >= COURT_WIDTH - 1) {
            if (ball_x <= 0) {
                score2 += 1;
            }
            else {
                score1 += 1;
            }
            ball_x = COURT_WIDTH / 2;
            ball_y = COURT_HEIGHT / 2;
        }
        if (ball_y <= 0 || ball_y >= COURT_HEIGHT - 1) {
            ball_vy = -ball_vy;
        }


        // Handle paddle controls
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 'w' && paddle1_y > 0) paddle1_y--;
            if (ch == 's' && paddle1_y < COURT_HEIGHT - PADDLE_HEIGHT) paddle1_y++;
            if (ch == 'i' && paddle2_y > 0) paddle2_y--;
            if (ch == 'k' && paddle2_y < COURT_HEIGHT - PADDLE_HEIGHT) paddle2_y++;
            if (ch == 'p') {
                special_power_active = !special_power_active;
            }
            if (ch == 'q') running = false;
        }

        // Control game speed
#ifdef _WIN32
        Sleep(100);
#else
        usleep(100000);
#endif
    }
}

int main() {
    std::cout << "Tennis for Two (Text Version with Controls)\n";
    std::cout << "Controls: W/S for left paddle, I/K for right paddle. Press 'q' to quit.\n";

    gameLoop();

    return 0;
}
