#pragma once
#include <string>

enum class TokenType {
    WORD,
    IO_NUMBER,
    OPERATOR
};

struct Token {
    std::string value;
    TokenType type;
};