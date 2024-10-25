#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>

// 插入排序
void insertionSort(std::vector<int>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// 归并排序的合并函数
void merge(std::vector<int>& arr, int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    std::vector<int> L(n1), R(n2);

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// 归并排序
void mergeSort(std::vector<int>& arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

// 打印数组
void printArray(const std::vector<int>& arr) {
    for (int num : arr) {
        std::cout << num << " ";
    }
    std::cout << "\n";
}

int main() {
    const int SIZE = 10000; // 数组大小
    std::vector<int> arr(SIZE);

    // 初始化随机数发生器
    srand(time(NULL));
    for (int i = 0; i < SIZE; i++) {
        arr[i] = rand() % SIZE; // 随机数
    }

    // 复制数组用于插入排序
    std::vector<int> arrInsertion = arr;

    // 测量插入排序时间
    auto start = std::chrono::high_resolution_clock::now();
    insertionSort(arrInsertion);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> insertionTime = end - start;
    std::cout << "Insertion Sort took " << insertionTime.count() << " milliseconds.\n";

    // 复制数组用于归并排序
    std::vector<int> arrMerge = arr;

    // 测量归并排序时间
    start = std::chrono::high_resolution_clock::now();
    mergeSort(arrMerge, 0, SIZE - 1);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> mergeTime = end - start;
    std::cout << "Merge Sort took " << mergeTime.count() << " milliseconds.\n";

    return 0;
}
