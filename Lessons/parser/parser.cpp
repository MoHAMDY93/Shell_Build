
#include <bits/stdc++.h>
#include "parser.h"

using namespace std;

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
