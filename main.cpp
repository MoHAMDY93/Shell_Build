#include <bits/stdc++.h>

using namespace std;

#include "Lessons/tokenizer/tokenizer.h"
#include "Lessons/parser/parser.h"


int main() {
    string line;

    while (getline(cin, line)) {
        vector<Token> tokens;

        try {
            tokens = tokenize(line);
        }
        catch (const exception& e) {
            cout << "ERR " << e.what() << '\n';
            continue;
        }

        if (tokens.empty())
            continue;

        try {
            SimpleCommand command = parse(tokens);

            cout << "argv:";
            for (const string& arg : command.args)
                cout << " [" << arg << "]";
            cout << '\n';

            for (const Redirection& r : command.redirections) {
                cout << "redir fd=" << r.fd
                     << " op=" << r.op
                     << " target=" << r.target << '\n';
            }
        }
        catch (const exception& e) {
            cout << "ERR " << e.what() << '\n';
        }
    }

    return 0;
}