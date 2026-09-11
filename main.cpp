#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

string trim(const string& s) {
    size_t first = s.find_first_not_of(" \t");
    if (first == string::npos) return "";
    size_t last = s.find_last_not_of(" \t");
    return s.substr(first, (last - first + 1));
}

int main() {
    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;
        vector<string> out;
        stringstream ss(line);
        string item;
        bool bad = false;
        while (getline(ss, item, '|')) {
            string trimmed = trim(item);
            if (trimmed.empty()) {
                bad = true;
                break;
            }
            out.push_back(trimmed);
        }

        if (!line.empty()) {
            size_t last_non_space = line.find_last_not_of(" \t");
            if (last_non_space != string::npos && line[last_non_space] == '|') {
                bad = true;
            }
        }

        if (bad || out.empty()) {
            cout << "ERR syntax error: empty command in pipeline\n";
            continue;
        }

        for (size_t i = 0; i < out.size(); ++i) {
            cout << out[i] << (i + 1 < out.size() ? " | " : "");
        }
        cout << '\n';
    }
    return 0;
}