#include <bits/stdc++.h>
using namespace std;

// TODO (tokenize): implement per the lesson description.

enum State {
    NORMAL,
    SINGLE_QUOTE,
    DOUBLE_QUOTE
};
int main() {
    string line;
    while (getline(cin, line)) {
        vector<string> tokens;
        State state = NORMAL;
        bool token_started = false , escaped = false;
        string curr_token;
        for (char c : line) {
            switch (state) {
            case NORMAL:
                if (escaped) {
                    curr_token.push_back(c);
                    token_started = true;
                    escaped = false;
                }
                else if (c == '\\') {
                    escaped = true;
                    token_started = true;
                }
                else if (c == ' ') {
                    if (token_started) {
                        tokens.push_back(curr_token);
                        curr_token.clear();
                        token_started = false;
                    }
                } 
                else if (c == '>' || c == '<' || c == '|') {
                    if (token_started) {
                        tokens.push_back(curr_token);
                        curr_token.clear();
                        token_started = false;
                    }
                    tokens.push_back(string(1 , c));
                } 
                else if (c == '\'') {
                    state = SINGLE_QUOTE;
                    token_started = true;
                } 
                else if (c == '\"') {
                    state = DOUBLE_QUOTE;
                    token_started = true;
                } 
                else {
                    curr_token.push_back(c);
                    token_started = true;
                }
                break;
            
            case SINGLE_QUOTE:
                if (c == '\'') {
                    state = NORMAL;
                } else {
                    curr_token.push_back(c);
                }
                break;
            case DOUBLE_QUOTE:
                if (escaped) {
                    curr_token.push_back(c);
                    token_started = true;
                    escaped = false;
                }
                else if (c == '\\') {
                    escaped = true;
                }
                else if (c == '\"') {
                    state = NORMAL;
                } 
                else {
                    curr_token.push_back(c);
                }
            }
        }
        if (state == SINGLE_QUOTE || state == DOUBLE_QUOTE) {
            cout << "ERR unterminated quote\n";
            return 0;
        } else if (token_started) {
            tokens.push_back(curr_token);
        }
    
        for (int i=0 ; i<tokens.size() ; i++) 
            cout << '[' << tokens[i] << ']' << (i + 1 < tokens.size() ? ' ' : '\n');
    }
}
