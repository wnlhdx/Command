#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定义正则表达式状态
typedef enum {
    STATE_START,
    STATE_MATCH,
    STATE_END
} State;

// 定义正则表达式节点
typedef struct RegexNode {
    State state;
    struct RegexNode* next;
} RegexNode;

// 创建正则表达式
RegexNode* createRegex(const char* pattern) {
    RegexNode* head = (RegexNode*)malloc(sizeof(RegexNode));
    head->state = STATE_START;
    head->next = NULL;

    RegexNode* current = head;
    for (int i = 0; pattern[i] != '\0'; ++i) {
        RegexNode* newNode = (RegexNode*)malloc(sizeof(RegexNode));
        newNode->state = STATE_MATCH;
        newNode->next = NULL;
        current->next = newNode;
        current = newNode;
    }

    RegexNode* endNode = (RegexNode*)malloc(sizeof(RegexNode));
    endNode->state = STATE_END;
    endNode->next = NULL;
    current->next = endNode;

    return head;
}

// 释放正则表达式
void freeRegex(RegexNode* regex) {
    RegexNode* temp;
    while (regex != NULL) {
        temp = regex;
        regex = regex->next;
        free(temp);
    }
}

// 执行正则表达式匹配
int matchRegex(RegexNode* regex, const char* text) {
    RegexNode* current = regex;
    while (*text != '\0') {
        if (current->state == STATE_START) {
            current = current->next;
        }
        else if (current->state == STATE_MATCH) {
            if (*text == current->next->state) {
                current = current->next;
            }
            else {
                return 0; // 匹配失败
            }
        }
        else if (current->state == STATE_END) {
            return 1; // 匹配成功
        }
        ++text;
    }
    return 0; // 匹配失败
}

/* 
* 主函数 测试一个abbbb的字符串是否匹配模式a*b
*/

int main() {
    const char* pattern = "a*b";
    RegexNode* regex = createRegex(pattern);
    const char* text = "abbbb";

    int result = matchRegex(regex, text);
    printf("匹配结果: %d\n", result);

    freeRegex(regex);
    return 0;
}
