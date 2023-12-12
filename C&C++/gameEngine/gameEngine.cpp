#include <iostream>
#include <string>

class Engine {
public:
  Engine(const std::string& title, int width, int height) {
    // 初始化游戏引擎
    std::cout << "Initializing engine..." << std::endl;
    _title = title;
    _width = width;
    _height = height;
  }

  void run() {
    // 运行游戏引擎
    std::cout << "Running engine..." << std::endl;
  }

private:
  std::string _title;
  int _width;
  int _height;
};

int main() {
  Engine engine("My Game", 800, 600);
  engine.run();
  return 0;
}