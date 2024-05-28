#include <iostream>
#include <string>

// 定义状态
enum State {
    START,
    ONE,
    TWO,
    END
};

// 有限状态机类
class FiniteStateMachine {
private:
    State currentState;

public:
    FiniteStateMachine() : currentState(START) {}

    // 输入处理函数
    void processInput(char input) {
        switch (currentState) {
        case START:
            if (input == '1') {
                currentState = ONE;
            }
            break;
        case ONE:
            if (input == '1') {
                currentState = TWO;
            }
            else {
                currentState = START;
            }
            break;
        case TWO:
            if (input == '1') {
                currentState = TWO;
            }
            else {
                currentState = START;
            }
            break;
        case END:
            // 最终状态，不处理输入
            break;
        }
    }

    // 获取当前状态
    State getCurrentState() const {
        return currentState;
    }

    // 检查是否到达接受状态
    bool isAccepted() const {
        return currentState == TWO;
    }
};

int main() {
    FiniteStateMachine fsm;
    std::string input;

    std::cout << "Enter a binary string: ";
    std::cin >> input;

    for (char c : input) {
        fsm.processInput(c);
    }

    if (fsm.isAccepted()) {
        std::cout << "The input string is accepted." << std::endl;
    }
    else {
        std::cout << "The input string is not accepted." << std::endl;
    }

    return 0;
}
