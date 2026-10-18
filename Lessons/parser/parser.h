#pragma once
#include <string>
#include <vector>
#include "../tokenizer/token.h"

struct Redirection {
    int fd;
    std::string op;
    std::string target;
};

struct SimpleCommand {
    std::vector<std::string> args;
    std::vector<Redirection> redirections;
};

SimpleCommand parse(const std::vector<Token>& tokens);