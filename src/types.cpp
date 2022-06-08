#include "types.hpp"
#include <string>


// -----------------------------------------------------------
// INT
// -----------------------------------------------------------

Int::Int() {
    this->is_null = true;
}

Int::Int(int value) {
    this->name = std::to_string(value);
    this->value = value;
    this->is_null = false;
}

std::string Int::getStr() {
    if (is_null) {
        return "Null";
    }

    return std::to_string(value);
}

// -----------------------------------------------------------
// CHAR
// -----------------------------------------------------------

Char::Char() {
    this->is_null = true;
}

Char::Char(char value) {
    this->name = std::to_string(value);
    this->value = value;
    this->is_null = false;
}

std::string Char::getStr() {
    if (is_null) {
        return "Null";
    }

    return std::to_string(value);
}

// -----------------------------------------------------------
// FLOAT
// -----------------------------------------------------------

Float::Float() {
    this->is_null = true;
}

Float::Float(float value) {
    this->name = std::to_string(value);
    this->value = value;
    this->is_null = false;
}

std::string Float::getStr() {
    if (is_null) {
        return "Null";
    }

    return std::to_string(value);
}