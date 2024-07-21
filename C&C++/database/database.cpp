#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <variant>

using namespace std;

// 定义一个类型别名，支持多种数据类型
using Value = variant<int, double, string>;

// 打印variant类型的函数
ostream& operator<<(ostream& os, const Value& value) {
    visit([&os](auto&& arg) { os << arg; }, value);
    return os;
}

class database {
private:
    map<string, Value> data;
    string filename;

public:
    database(const string& file) : filename(file) {
        load();
    }

    ~database() {
        save();
    }

    template<typename T>
    void insert(const string& key, T value) {
        data[key] = value;
    }

    string retrieve(const string& key) {
        auto it = data.find(key);
        if (it != data.end()) {
            stringstream ss;
            ss << it->second;
            return ss.str();
        }
        else {
            return "Key Not Found";
        }
    }

    void remove(const string& key) {
        auto it = data.find(key);
        if (it != data.end()) {
            data.erase(it);
            cout << "Key removed successfully" << endl;
        }
        else {
            cout << "Key Not Found" << endl;
        }
    }

private:
    void save() {
        ofstream out(filename);
        if (!out) {
            cerr << "Unable to open file for writing" << endl;
            return;
        }
        for (const auto& [key, value] : data) {
            stringstream ss;
            ss << value;
            out << key << "=" << ss.str() << endl;
        }
    }

    void load() {
        ifstream in(filename);
        if (!in) {
            cerr << "No existing database found, starting a new one." << endl;
            return;
        }
        string line;
        while (getline(in, line)) {
            size_t pos = line.find('=');
            if (pos != string::npos) {
                string key = line.substr(0, pos);
                string valueStr = line.substr(pos + 1);
                Value value;
                if (istringstream(valueStr) >> value) {
                    data[key] = value;
                }
            }
        }
    }
};

int main() {
    database db("database.txt");
    db.insert("name", "John");
}