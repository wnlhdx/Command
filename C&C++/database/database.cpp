#include <iostream>
#include <map>
#include <string>

using namespace std;

class database {
private:
	map<string, string> data;

public:
	void insert(const string& key, const string& value) {
		data[key] = value;
	}

	string retrieve(const string& key) {
		if (data.find(key) != data.end()) {
			return data[key];
		}
		else {
			return "Key Not Found";
		}
	}

	void remove(const string& key) {
		if (data.find(key) != data.end()) {
			data.erase(key);
			cout << "Key removed successfully" << endl;
		}
		else {
			cout << "Key Not Found" << endl;
		}
	}
};

	int main() {
		database db;
		db.insert("name", "John");
		db.insert("age", "30");
		cout << ("Name:") << db.retrieve("name") << endl;
		cout << ("Age:") << db.retrieve("age") << endl;

		db.remove("age");
		cout << ("Age:") << db.retrieve("age") << endl;
		return 0;
	}
