#include <iostream>
#include <conio.h> // 用于_getch()，仅适用于Windows

// 定义地图类
class Map {
public:
    static const int WIDTH = 11;
    static const int HEIGHT = 11;
    char grid[HEIGHT][WIDTH];

    Map() {
        // 初始化地图
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                grid[y][x] = (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1) ? '#' : '.';
            }
        }
    }

    void Draw() {
        // 绘制地图
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                std::cout << grid[y][x];
            }
            std::cout << std::endl;
        }
    }
};

// 定义玩家类
class Player {
public:
    int x;
    int y;

    Player(Map& map) : x(1), y(1) {
        // 将玩家放置在地图上
        map.grid[y][x] = 'P';
    }

    void Move(char key, Map& map) {
        // 保存玩家当前的位置
        int prevX = x;
        int prevY = y;

        // 根据按键移动玩家
        switch (key) {
        case 'w': // 上
        case 'W':
            y--;
            break;
        case 's': // 下
        case 'S':
            y++;
            break;
        case 'a': // 左
        case 'A':
            x--;
            break;
        case 'd': // 右
        case 'D':
            x++;
            break;
        default:
            return; // 如果不是移动键，则不进行任何操作
        }

        // 检查移动后的位置是否有效
        if (map.grid[y][x] != '#') {
            // 移动玩家
            map.grid[prevY][prevX] = '.'; // 清除原来的位置
            map.grid[y][x] = 'P'; // 设置新的位置
        }
        else {
            // 如果移动到的位置是墙，则取消移动
            x = prevX;
            y = prevY;
        }
    }
};

// 游戏主循环
void gameLoop(Map& map, Player& player) {
    bool running = true;
    while (running) {
        // 清屏
        system("cls"); // 仅适用于Windows

        // 绘制地图
        map.Draw();

        // 处理玩家输入
        char key = _getch(); // 等待按键输入
        player.Move(key, map);

        // 检查是否退出
        if (key == 'q' || key == 'Q') {
            running = false;
        }
    }
}

int main() {
    Map map;
    Player player(map);
    gameLoop(map, player);
    return 0;
}
