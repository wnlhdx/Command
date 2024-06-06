#include <iostream>
#include <vector>
using namespace std;

class MyArray {
private:
    vector<int> data; // 使用vector来实现动态数组

public:
    // 添加元素
    void add(int value) {
        data.push_back(value);
    }

    // 获取元素
    int get(int index) {
        if (index >= 0 && index < data.size()) {
            return data[index];
        }
        else {
            cout << "Index out of bounds" << endl;
            return -1; // 或者抛出异常
        }
    }

    // 打印数组
    void print() {
        for (int i = 0; i < data.size(); i++) {
            cout << data[i] << " ";
        }
        cout << endl;
    }

    // 获取数组大小
    int size() {
        return data.size();
    }
};

int main() {
    MyArray arr;

    arr.add(1);
    arr.add(2);
    arr.add(3);
    arr.add(4);
    arr.add(5);

    arr.print(); // 输出: 1 2 3 4 5

    cout << "Element at index 2: " << arr.get(2) << endl; // 输出: Element at index 2: 3

    return 0;
}
