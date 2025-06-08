#include <iostream>
#include <vector>
#include <chrono>

// 线性搜索
int linearSearch(const std::vector<int>& arr, int target) {
    for (size_t i = 0; i < arr.size(); ++i) {
        if (arr[i] == target) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// 二分查找（有序数组）
int binarySearch(const std::vector<int>& arr, int target) {
    int left = 0, right = static_cast<int>(arr.size()) - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] == target) return mid;
        else if (arr[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

int main() {
    // 构造有序数据
    std::vector<int> data;
    for (int i = 0; i < 1000000; ++i) {
        data.push_back(i * 2);  // 偶数序列，有序
    }

    int target = 1999998;  // 目标值

    // 线性搜索计时
    auto start_linear = std::chrono::high_resolution_clock::now();
    int index_linear = linearSearch(data, target);
    auto end_linear = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_linear = end_linear - start_linear;

    // 二分查找计时
    auto start_binary = std::chrono::high_resolution_clock::now();
    int index_binary = binarySearch(data, target);
    auto end_binary = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_binary = end_binary - start_binary;

    // 输出结果和时间
    if (index_linear != -1) {
        std::cout << "[线性搜索] 找到目标 " << target << "，索引为 " << index_linear << "\n";
    } else {
        std::cout << "[线性搜索] 未找到目标 " << target << "\n";
    }
    std::cout << "线性搜索耗时: " << elapsed_linear.count() << " 秒\n";

    if (index_binary != -1) {
        std::cout << "[二分查找] 找到目标 " << target << "，索引为 " << index_binary << "\n";
    } else {
        std::cout << "[二分查找] 未找到目标 " << target << "\n";
    }
    std::cout << "二分查找耗时: " << elapsed_binary.count() << " 秒\n";

    return 0;
}
