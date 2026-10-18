#include <bits/stdc++.h>

using namespace std;

enum class TokenType {
    WORD,
    IO_NUMBER,
    OPERATOR
};

struct Token {
    std::string value;
    TokenType type;
};

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
                quoted = true; // A backslash acts as quotes for IO_NUMBER checks
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
                    // Check if the immediately touching token is an IO_NUMBER
                    bool is_io_number = false;
                    if (!quoted && !curr_token.empty() &&
                        all_of(curr_token.begin(), curr_token.end(), [](unsigned char ch) { return isdigit(ch); })) {
                        if (c == '<' || c == '>') {
                            is_io_number = true;
                        }
                    }

                    if (is_io_number) {
                        tokens.push_back({curr_token, TokenType::IO_NUMBER});
                    } else {
                        pushWord(tokens, curr_token);
                    }

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
        throw runtime_error("syntax error: trailing escape");
    }

    if (state != NORMAL) {
        throw runtime_error("unterminated quote");
    }

    if (token_started) {
        pushWord(tokens, curr_token);
    }

    return tokens;
}


struct Redirection {
    int fd;
    std::string op;
    std::string target;
};

struct SimpleCommand {
    std::vector<std::string> args;
    std::vector<Redirection> redirections;
};

bool isRedirection(const string& op) {
    return op == "<"  || op == ">"  ||
           op == ">>" || op == "<<" ||
           op == "<<-" || op == ">&" ||
           op == "<&";
}

int defaultFd(const string& op) {
    return op[0] == '<' ? 0 : 1;
}

SimpleCommand parse(const vector<Token>& tokens) {
    SimpleCommand command;
    int pendingFd = -1;

    for (size_t i = 0; i < tokens.size(); ++i) {
        const Token& token = tokens[i];

        // Ordinary words become arguments.
        if (token.type == TokenType::WORD) {
            if (pendingFd != -1) {
                throw runtime_error(
                    "syntax error near unexpected token '" +
                    token.value + "'"
                );
            }

            command.args.push_back(token.value);
        }

        // Remember the descriptor for the next redirection.
        else if (token.type == TokenType::IO_NUMBER) {
            if (pendingFd != -1) {
                throw runtime_error(
                    "syntax error near unexpected token '" +
                    token.value + "'"
                );
            }

            try {
                size_t pos;
                int fd = stoi(token.value, &pos);

                if (pos != token.value.size() || fd < 0) {
                    throw runtime_error("invalid file descriptor");
                }

                pendingFd = fd;
            }
            catch (const exception&) {
                throw runtime_error(
                    "syntax error near unexpected token '" +
                    token.value + "'"
                );
            }
        }

        // Process operators.
        else if (token.type == TokenType::OPERATOR) {
            // Unsupported operators, e.g. |, ;, &&.
            if (!isRedirection(token.value)) {
                throw runtime_error(
                    "syntax error near unexpected token '" +
                    token.value + "'"
                );
            }

            // An IO number must be followed by its redirection.
            int fd = pendingFd == -1
                   ? defaultFd(token.value)
                   : pendingFd;

            pendingFd = -1;

            // Redirection operator must have a target.
            if (i + 1 >= tokens.size()) {
                throw runtime_error(
                    "syntax error near unexpected token 'newline'"
                );
            }

            const Token& target = tokens[i + 1];

            // Another operator cannot serve as a target.
            if (target.type == TokenType::OPERATOR) {
                throw runtime_error(
                    "syntax error near unexpected token '" +
                    target.value + "'"
                );
            }

            // An IO number is not a valid target in this exercise.
            if (target.type != TokenType::WORD) {
                throw runtime_error(
                    "syntax error near unexpected token '" +
                    target.value + "'"
                );
            }

            command.redirections.push_back({
                fd,
                token.value,
                target.value
            });

            // Consume the target so it isn't treated as an argument.
            ++i;
        }
    }

    // A descriptor without a following redirection is invalid.
    if (pendingFd != -1) {
        throw runtime_error(
            "syntax error near unexpected token 'newline'"
        );
    }

    return command;
}


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