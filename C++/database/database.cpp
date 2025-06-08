#include <iostream>
#include <map>
#include <fstream>
#include <variant>
#include <string>
#include <sstream>

using namespace std;

using Value = variant<int, double, string>;

ostream& operator<<(ostream& os, const Value& value) {
    visit([&os](auto&& arg) { os << arg; }, value);
    return os;
}

// 简化的“数据库引擎”示意结构
class SimpleDatabaseEngine {
private:
    map<string, Value> data;
    string filename;

public:
    SimpleDatabaseEngine(const string& fname) : filename(fname) {
        load();
    }

    void insert(const string& key, const Value& value) {
        data[key] = value;
    }

    string get(const string& key) {
        if (data.count(key)) {
            stringstream ss;
            ss << data[key];
            return ss.str();
        }
        return "Not Found";
    }

    void remove(const string& key) {
        data.erase(key);
    }

    void save() {
        ofstream out(filename);
        for (auto& [k, v] : data) {
            stringstream ss;
            ss << v;
            out << k << "=" << ss.str() << endl;
        }
    }

    void load() {
        ifstream in(filename);
        string line;
        while (getline(in, line)) {
            auto pos = line.find('=');
            if (pos == string::npos) continue;
            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);
            // 只判断数字，不复杂推断类型
            if (all_of(value.begin(), value.end(), ::isdigit)) {
                data[key] = stoi(value);
            } else if (value.find('.') != string::npos) {
                data[key] = stod(value);
            } else {
                data[key] = value;
            }
        }
    }

    ~SimpleDatabaseEngine() {
        save();  // 自动保存
    }
};

int main() {
    SimpleDatabaseEngine db("data.txt");
    db.insert("name", "Alice");
    db.insert("age", 22);
    db.insert("gpa", 3.85);
    cout << db.get("name") << endl;
    db.remove("age");
    return 0;
}
