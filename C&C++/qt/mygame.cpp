#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <QKeyEvent>

class GameWindow : public QWidget {
public:
    GameWindow(QWidget* parent = nullptr) : QWidget(parent) {
        setFixedSize(800, 600);
        setWindowTitle("My Game");
        setWindowIcon(QIcon("icon.png")); // 假设您有一个名为 icon.png 的图标文件
    }

    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.fillRect(rect(), QColor(0, 0, 0)); // 填充窗口为黑色
        // 在这里添加绘制游戏元素（例如：玩家角色、地图等）的代码
    }

    void keyPressEvent(QKeyEvent* event) override {
        switch (event->key()) {
        case Qt::Key_Left:
            // 处理左键按下
            break;
        case Qt::Key_Right:
            // 处理右键按下
            break;
        case Qt::Key_Up:
            // 处理上键按下
            break;
        case Qt::Key_Down:
            // 处理下键按下
            break;
        default:
            // 处理其他按键
            break;
        }
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    GameWindow window;
    window.show();
    return app.exec();
}
