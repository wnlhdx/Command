#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

//定义一个结构体用于存储数据
struct Data {
    int id;
    string name;
    int age;
};

//定义一个存储引擎类
class StorageEngine {
private:
    vector<Data> data_; //数据存储在vector容器中

public:
    void insertData(Data data) {
        data_.push_back(data); //插入数据到vector容器中
    }

    void deleteData(int id) {
        //使用erase和remove_if函数从vector容器中删除指定id的数据
        data_.erase(remove_if(data_.begin(), data_.end(), [id](const Data& data) {
            return data.id == id;
        }), data_.end());
    }

    vector<Data> getAllData() {
        return data_; //返回所有数据
    }

    void saveToFile(const string& filename) {
        //将vector容器中的数据写入到二进制文件中
        ofstream file(filename, ios::out | ios::binary);
        for (const auto& data : data_) {
            file.write((char*)&data, sizeof(Data));
        }
        file.close();
    }

    void loadFromFile(const string& filename) {
        //从二进制文件中读取数据，并存储到vector容器中
        ifstream file(filename, ios::in | ios::binary);
        Data data;
        while (file.read((char*)&data, sizeof(Data))) {
            data_.push_back(data);
        }
        file.close();
    }
};

//测试代码
int main() {
    StorageEngine engine;

    //插入数据
    Data data1 = { 1, "Tom", 20 };
    Data data2 = { 2, "Jerry", 25 };
    engine.insertData(data1);
    engine.insertData(data2);

    //保存数据到文件
    engine.saveToFile("data.bin");

    //从文件中读取数据
    engine.loadFromFile("data.bin");

    //输出所有数据
    vector<Data> allData = engine.getAllData();
    for (const auto& data : allData) {
        cout << "id: " << data.id << ", name: " << data.name << ", age: " << data.age << endl;
    }

    //删除数据
    engine.deleteData(1);

    //输出剩余数据
    allData = engine.getAllData();
    for (const auto& data : allData) {
        cout << "id: " << data.id << ", name: " << data.name << ", age: " << data.age << endl;
    }

    return 0;
}