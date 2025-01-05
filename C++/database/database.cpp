#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <variant>
#include <vector>
#include <functional>
#include <stdexcept>

using namespace std;

// 定义一个类型别名，支持多种数据类型
using Value = variant<int, double, string>;

// 打印variant类型的函数
ostream& operator<<(ostream& os, const Value& value) {
    visit([&os](auto&& arg) { os << arg; }, value);
    return os;
}

// 数据字段类型枚举
enum class DataType { INT, DOUBLE, STRING };

// 字段结构体
struct Field {
    string name;
    DataType type;
};

// 数据库类
class Database {
private:
    map<string, Value> data;  // 存储键值对数据
    map<string, string> views; // 存储视图
    string filename;

public:
    Database(const string& file) : filename(file) {
        load();
    }

    ~Database() {
        save();
    }

    // 插入数据，支持不同类型
    template <typename T>
    void insert(const string& key, T value) {
        data[key] = value;
    }

    // 查询数据
    string retrieve(const string& key) {
        auto it = data.find(key);
        if (it != data.end()) {
            stringstream ss;
            ss << it->second;
            return ss.str();
        }
        return "Key Not Found";
    }

    // 删除数据
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

    // 创建视图
    void createView(const string& viewName, const string& query) {
        views[viewName] = query;
        cout << "View " << viewName << " created successfully" << endl;
    }

    // 查询视图
    void queryView(const string& viewName) {
        if (views.find(viewName) != views.end()) {
            executeSQL(views[viewName]);
        }
        else {
            cout << "View " << viewName << " not found." << endl;
        }
    }

    // 执行 SQL 查询
    void executeSQL(const string& query) {
        stringstream ss(query);
        string command;
        ss >> command;

        if (command == "INSERT") {
            string key, value;
            ss >> key >> value;
            insert(key, value);
            cout << "Inserted: " << key << " = " << value << endl;
        }
        else if (command == "SELECT") {
            string key;
            ss >> key;
            cout << "Retrieved: " << retrieve(key) << endl;
        }
        else if (command == "DELETE") {
            string key;
            ss >> key;
            remove(key);
        }
        else {
            cout << "Invalid SQL command!" << endl;
        }
    }

    // 打印数据
    void printData() {
        for (const auto& [key, value] : data) {
            cout << key << " = " << value << endl;
        }
    }

private:
    // 保存数据到文件
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

    // 从文件加载数据
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

// 表类：支持表结构和类型验证
class Table {
private:
    string name;
    vector<Field> fields;
    map<string, map<string, Value>> rows;  // 存储表数据，使用key作为行标识

public:
    Table(const string& tableName) : name(tableName) {}

    void addField(const string& fieldName, DataType fieldType) {
        fields.push_back({ fieldName, fieldType });
    }

    // 插入行数据
    bool insertRow(const map<string, Value>& rowData) {
        for (const auto& field : fields) {
            auto it = rowData.find(field.name);
            if (it != rowData.end()) {
                const Value& value = it->second;
                if (!validateType(value, field.type)) {
                    cout << "Invalid data type for field: " << field.name << endl;
                    return false;
                }
            }
        }
        rows[to_string(rows.size() + 1)] = rowData;  // 使用当前行数作为行标识
        return true;
    }

    // 验证数据类型
    bool validateType(const Value& value, DataType expectedType) {
        if (expectedType == DataType::INT && holds_alternative<int>(value)) return true;
        if (expectedType == DataType::DOUBLE && holds_alternative<double>(value)) return true;
        if (expectedType == DataType::STRING && holds_alternative<string>(value)) return true;
        return false;
    }

    // 查询某行数据
    string retrieveRow(const string& key) {
        auto it = rows.find(key);
        if (it != rows.end()) {
            stringstream ss;
            for (const auto& field : fields) {
                ss << field.name << ": " << it->second.at(field.name) << " ";
            }
            return ss.str();
        }
        return "Row Not Found";
    }

    void printTable() {
        cout << "Table: " << name << endl;
        for (const auto& row : rows) {
            cout << "Row " << row.first << ": ";
            for (const auto& field : fields) {
                cout << field.name << ": " << row.second.at(field.name) << " ";
            }
            cout << endl;
        }
    }
};

// 事务类：简单事务支持
class Transaction {
private:
    vector<function<void()>> operations;

public:
    void addOperation(const function<void()>& operation) {
        operations.push_back(operation);
    }

    // 提交事务
    void commit() {
        for (auto& op : operations) {
            op();
        }
        operations.clear();
        cout << "Transaction committed." << endl;
    }

    // 回滚事务
    void rollback() {
        operations.clear();
        cout << "Transaction rolled back." << endl;
    }
};

// 主程序
int main() {
    // 创建数据库对象，加载数据库文件
    Database db("database.txt");

    // 插入数据
    db.insert("name", "John");
    db.insert("age", 30);
    db.insert("salary", 50000.50);

    // 执行SQL命令
    db.executeSQL("INSERT name John");
    db.executeSQL("SELECT name");
    db.executeSQL("DELETE name");

    // 打印所有数据
    db.printData();

    // 创建和查询视图
    db.createView("view1", "SELECT name");
    db.queryView("view1");

    // 创建表并插入数据
    Table userTable("Users");
    userTable.addField("name", DataType::STRING);
    userTable.addField("age", DataType::INT);
    userTable.addField("salary", DataType::DOUBLE);

    map<string, Value> row1 = { {"name", "Alice"}, {"age", 28}, {"salary", 45000.75} };
    userTable.insertRow(row1);

    map<string, Value> row2 = { {"name", "Bob"}, {"age", 35}, {"salary", 60000.00} };
    userTable.insertRow(row2);

    // 打印表数据
    userTable.printTable();

    // 事务操作
    Transaction txn;
    txn.addOperation([&] { db.insert("name", "Charlie"); });
    txn.addOperation([&] { db.insert("age", 25); });
    txn.commit();

    return 0;
}
