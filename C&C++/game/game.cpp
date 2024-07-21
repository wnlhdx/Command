#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>

#if defined(_WIN32) || defined(_WIN64)
#include <conio.h> // For _kbhit() and _getch() on Windows
#else
#include <termios.h>
#include <unistd.h>

// Function to get a single character from terminal without waiting for Enter
char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

// Define the dimensions of the board
const int WIDTH = 11;
const int HEIGHT = 15;
const int START_POS = WIDTH / 2;

class Bagatelle {
public:
    Bagatelle();
    void drawBoard();
    void releaseBall();
    void run();

private:
    std::vector<std::vector<char>> board;
    int score;
    int power;
    void initializeBoard();
};

Bagatelle::Bagatelle() : board(HEIGHT, std::vector<char>(WIDTH, ' ')), score(0), power(1) {
    initializeBoard();
}

void Bagatelle::initializeBoard() {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (y % 2 == 0 && x % 2 == 0) {
                board[y][x] = 'O'; // Pegs
            }
            else {
                board[y][x] = ' ';
            }
        }
    }
    // Add pockets at the bottom
    for (int x = 1; x < WIDTH; x += 2) {
        board[HEIGHT - 1][x] = 'U'; // Pockets
    }
}

void Bagatelle::drawBoard() {
    for (const auto& row : board) {
        for (const auto& cell : row) {
            std::cout << cell;
        }
        std::cout << '\n';
    }
    std::cout << "Score: " << score << "\n";
    std::cout << "Power: " << power << "\n";
}

void Bagatelle::releaseBall() {
    int x = START_POS;
    int y = 0;
    while (y < HEIGHT - 1) {
        board[y][x] = ' ';
        y++;
        if (board[y][x] == 'O') {
            // Ball hits a peg
            x += (std::rand() % 2 == 0) ? -1 : 1;
        }
        if (x < 0) x = 0;
        if (x >= WIDTH) x = WIDTH - 1;
        board[y][x] = 'B';
        std::this_thread::sleep_for(std::chrono::milliseconds(100 / power));
    }
    // Check where the ball lands
    if (board[y][x] == 'U') {
        score += 10 * power;
    }
    board[y][x] = ' ';
}

void Bagatelle::run() {
    std::srand(std::time(nullptr));
    char choice;
    do {
        system("clear"); // Clear the screen
        drawBoard();
        std::cout << "Press 'w' to increase power, 's' to decrease power, 'r' to release the ball, 'q' to quit.\n";
#if defined(_WIN32) || defined(_WIN64)
        choice = _getch();
#else
        choice = getch();
#endif
        switch (choice) {
        case 'w':
            if (power < 10) power++;
            break;
        case 's':
            if (power > 1) power--;
            break;
        case 'r':
            releaseBall();
            break;
        case 'q':
            break;
        default:
            break;
        }
    } while (choice != 'q');
}

int main() {
    Bagatelle game;
    game.run();
    return 0;
}
