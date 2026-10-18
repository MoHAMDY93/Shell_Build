#include <bits/stdc++.h>
#include "tokenizer.h"

using namespace std;

enum State {
    NORMAL,
    SINGLE_QUOTE,
    DOUBLE_QUOTE
};

static bool isOperatorChar(char c) {
    return c == '<' || c == '>' || c == '|' ||
           c == ';' || c == '&';
}

static void pushWord(vector<Token>& tokens, const string& word) {
    tokens.push_back({word, TokenType::WORD});
}

vector<Token> tokenize(const string& line) {
    vector<Token> tokens;

    State state = NORMAL;
    bool token_started = false;
    bool escaped = false;
    bool quoted = false;
    string curr_token;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

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
            else if (c == ' ' || c == '\t') {
                if (token_started) {
                    pushWord(tokens, curr_token);
                    curr_token.clear();
                    token_started = false;
                    quoted = false;
                }
            }
            else if (isOperatorChar(c)) {
                if (token_started) {
                    pushWord(tokens, curr_token);
                    curr_token.clear();
                    token_started = false;
                    quoted = false;
                }

                // Combine multi-character operators.
                string op(1, c);

                if (i + 1 < line.size()) {
                    char next = line[i + 1];

                    if ((c == '>' && (next == '>' || next == '&')) ||
                        (c == '<' && (next == '<' || next == '&')) ||
                        (c == '|' && next == '|') ||
                        (c == '&' && next == '&')) {
                        op.push_back(next);
                        ++i;

                        // Recognize <<- as one operator.
                        if (op == "<<" && i + 1 < line.size() &&
                            line[i + 1] == '-') {
                            op.push_back('-');
                            ++i;
                        }
                    }
                }

                tokens.push_back({op, TokenType::OPERATOR});
            }
            else if (c == '\'') {
                state = SINGLE_QUOTE;
                token_started = true;
                quoted = true;
            }
            else if (c == '"') {
                state = DOUBLE_QUOTE;
                token_started = true;
                quoted = true;
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
                escaped = false;
            }
            else if (c == '\\') {
                escaped = true;
            }
            else if (c == '"') {
                state = NORMAL;
            }
            else {
                curr_token.push_back(c);
            }
            break;
        }
    }

    if (escaped) {
        // A trailing backslash is a syntax error in this simplified lexer.
        throw runtime_error("syntax error: trailing escape");
    }

    if (state != NORMAL) {
        throw runtime_error("unterminated quote");
    }

    if (token_started) {
        pushWord(tokens, curr_token);
    }

    // Mark an unquoted, all-digit word as an IO number only when
    // it touches a following redirection operator.
    for (size_t i = 0; i + 1 < tokens.size(); ++i) {
        if (tokens[i].type != TokenType::WORD ||
            tokens[i + 1].type != TokenType::OPERATOR) {
            continue;
        }

        const string& op = tokens[i + 1].value;
        if (op != "<" && op != ">" && op != ">>" &&
            op != "<<" && op != "<<-" && op != ">&" &&
            op != "<&") {
            continue;
        }

        const string& word = tokens[i].value;
        bool digits = !word.empty() &&
            all_of(word.begin(), word.end(), [](unsigned char ch) {
                return isdigit(ch);
            });

        // Quoted numeric words must not become IO_NUMBER tokens.
        if (digits) {
            tokens[i].type = TokenType::IO_NUMBER;
        }
    }

    return tokens;
}
