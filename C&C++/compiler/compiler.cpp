#include <iostream>
#include <string>

using namespace std;

//定义token类型
enum class TokenType {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
};

//定义token结构体
struct Token {
    TokenType type;
    string value;
};

//定义词法分析器类
class Lexer {
private:
    string input_;
    size_t position_;

public:
    Lexer(const string& input) {
        input_ = input;
        position_ = 0;
    }

    Token getNextToken() {
        //跳过空格
        while (position_ < input_.size() && isspace(input_[position_])) {
            position_++;
        }

        //如果已经到达输入末尾，返回空的token
        if (position_ == input_.size()) {
            return { TokenType::NUMBER, "" };
        }

        //如果下一个字符是数字，则解析数字
        if (isdigit(input_[position_])) {
            string value;
            while (position_ < input_.size() && isdigit(input_[position_])) {
                value += input_[position_];
                position_++;
            }
            return { TokenType::NUMBER, value };
        }
        
        //如果下一个字符是加号，则返回加号token
        if (input_[position_] == '+') {
            position_++;
            return { TokenType::PLUS, "+" };
        }

        //如果下一个字符是减号，则返回减号token
        if (input_[position_] == '-') {
            position_++;
            return { TokenType::MINUS, "-" };
        }

        //如果下一个字符是乘号，则返回乘号token
        if (input_[position_] == '*') {
            position_++;
            return { TokenType::MULTIPLY, "*" };
        }

        //如果下一个字符是除号，则返回除号token
        if (input_[position_] == '/') {
            position_++;
            return { TokenType::DIVIDE, "/" };
        }

        //如果无法识别下一个字符，则抛出异常
        throw runtime_error("Invalid character");
    }
};

//测试代码
int main() {
    string input = "3 + 4 * 2 - 1 / 5";
    Lexer lexer(input);
    Token token;

    while (token.type != TokenType::NUMBER) {
        token = lexer.getNextToken();
        cout << "type: " << static_cast<int>(token.type) << ", value: " << token.value << endl;
    }

    return 0;
}