#pragma once

#include <cstddef>

struct Token {
    size_t tableID;
    size_t rowID;
};

enum TokenType {
    TokenOperation,
    TokenSeparator,
    TokenWord,
    TokenConstant,
    TokenConstantChar,
    TokenBracket,
    TokenError, // The correct name is TokenEmpty
};