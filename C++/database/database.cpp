#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <variant>
#include <string>
#include <iomanip>

using namespace std;

using Value = variant<int, double, string>;

ostream& operator<<(ostream& os, const Value& value) {
    visit([&os](auto&& arg) { os << arg; }, value);
    return os;
}

enum class FieldType { INT, DOUBLE, STRING };

struct Field {
    string name;
    FieldType type;
};

struct Table {
    vector<Field> schema;
    vector<vector<Value>> records;
};

class SimpleDB {
private:
    map<string, Table> tables;

public:
    void create_table(const string& table_name, const vector<Field>& schema) {
        if (tables.count(table_name)) {
            cout << "Table already exists.\n";
            return;
        }
        tables[table_name] = Table{ schema };
        cout << "Table '" << table_name << "' created.\n";
    }

    void insert_into(const string& table_name, const vector<Value>& values) {
        if (!tables.count(table_name)) {
            cout << "Table not found.\n";
            return;
        }
        auto& table = tables[table_name];
        if (values.size() != table.schema.size()) {
            cout << "Value count doesn't match table schema.\n";
            return;
        }
        table.records.push_back(values);
        cout << "Record inserted.\n";
    }

    void select_all(const string& table_name) {
        if (!tables.count(table_name)) {
            cout << "Table not found.\n";
            return;
        }

        const auto& table = tables[table_name];
        for (const auto& field : table.schema) {
            cout << setw(12) << field.name << " ";
        }
        cout << "\n---------------------------------------------\n";

        for (const auto& row : table.records) {
            for (const auto& val : row) {
                cout << setw(12) << val << " ";
            }
            cout << "\n";
        }
    }

    void save_to_csv(const string& table_name) {
        if (!tables.count(table_name)) return;

        ofstream out(table_name + ".csv");
        const auto& table = tables[table_name];

        // header
        for (size_t i = 0; i < table.schema.size(); ++i) {
            out << table.schema[i].name;
            if (i != table.schema.size() - 1) out << ",";
        }
        out << "\n";

        // data
        for (const auto& row : table.records) {
            for (size_t i = 0; i < row.size(); ++i) {
                visit([&out](auto&& arg) { out << arg; }, row[i]);
                if (i != row.size() - 1) out << ",";
            }
            out << "\n";
        }
    }

    void load_from_csv(const string& table_name, const vector<Field>& schema) {
        ifstream in(table_name + ".csv");
        if (!in) return;

        string line;
        getline(in, line); // skip header

        Table table{ schema };

        while (getline(in, line)) {
            stringstream ss(line);
            string cell;
            vector<Value> row;
            for (const auto& field : schema) {
                getline(ss, cell, ',');
                if (field.type == FieldType::INT) {
                    row.emplace_back(stoi(cell));
                } else if (field.type == FieldType::DOUBLE) {
                    row.emplace_back(stod(cell));
                } else {
                    row.emplace_back(cell);
                }
            }
            table.records.push_back(row);
        }
        tables[table_name] = table;
    }
};

int main() {
    SimpleDB db;

    vector<Field> student_schema = {
        {"id", FieldType::INT},
        {"name", FieldType::STRING},
        {"gpa", FieldType::DOUBLE}
    };

    db.create_table("students", student_schema);
    db.insert_into("students", {1, "Alice", 3.8});
    db.insert_into("students", {2, "Bob", 3.5});

    cout << "\nAll records in 'students':\n";
    db.select_all("students");

    db.save_to_csv("students");
}