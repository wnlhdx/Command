#include <iostream>
#include <string>
#include <vector>

using namespace std;

// 定义进程结构体
struct Process {
    int id;
    string name;
};

// 定义调度器类
class Scheduler {
private:
    vector<Process> processes_;

public:
    void addProcess(const Process& process) {
        processes_.push_back(process);
    }

    void removeProcess(int id) {
        for (auto it = processes_.begin(); it != processes_.end(); ++it) {
            if (it->id == id) {
                processes_.erase(it);
                break;
            }
        }
    }

    void run() {
        cout << "Running processes:" << endl;
        for (const auto& process : processes_) {
            cout << "ID: " << process.id << ", Name: " << process.name << endl;
        }
    }
};

// 测试代码
int main() {
    Scheduler scheduler;

    // 添加进程
    Process p1 = { 1, "Process 1" };
    Process p2 = { 2, "Process 2" };
    Process p3 = { 3, "Process 3" };
    scheduler.addProcess(p1);
    scheduler.addProcess(p2);
    scheduler.addProcess(p3);

    // 运行进程
    scheduler.run();

    // 移除进程
    scheduler.removeProcess(2);

    // 再次运行进程
    scheduler.run();

    return 0;
}