#include <iostream>
#include <string>
#include <memory> // 包含智能指针所需的头文件

// 定义双向链表类
class State {
public:
    int value;
    State* prev;
    State* next;

    State(int val) : value(val), prev(nullptr), next(nullptr) {}
};

// 有限状态机类
class FiniteStateMachine {
private:
    std::unique_ptr<State> currentState; // 使用智能指针管理内存

public:
    FiniteStateMachine() : currentState(new State(0)) {}

    // 输入处理函数
    void processInput(char input) {
        if (input == 'a') {
            currentState = std::make_unique<State>(currentState->value + 1);
            currentState->prev = currentState.get(); // 手动管理prev指针
        }
        else if (input == 'b') {
            currentState = std::make_unique<State>(currentState->value - 1);
            currentState->prev = currentState.get(); // 手动管理prev指针
        }
        else {
            throw std::invalid_argument("Input should only be 'a' and 'b'");
        }
    }

    // 获取当前状态
    const State* getCurrentState() const {
        return currentState.get();
    }

    // 检查是否到达接受状态
    bool isAccepted() const {
        return currentState->value > 0; // 修改为非零表示接受
    }
};

int main() {
    FiniteStateMachine fsm;
    std::string input;

    std::cout << "Enter a binary string: ";
    std::cin >> input;

    try {
        for (char c : input) {
            fsm.processInput(c);
        }

        if (fsm.isAccepted()) {
            std::cout << "The input string is accepted." << std::endl;
        }
        else {
            std::cout << "The input string is not accepted." << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Caught an exception: " << e.what() << std::endl;
    }

    // 释放链表中的所有State对象
    State* current = fsm.getCurrentState();
    while (current != nullptr) {
        State* next = current->next;
        delete current;
        current = next;
    }

    return 0;
}
