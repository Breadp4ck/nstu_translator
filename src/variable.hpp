#pragma once
#include <string>

#include "types.hpp"

class Variable {
    Type data;
    TypesID typeID;
    std::string varName;

public:
    Variable(std::string varName);
    std::string name();
    TypesID type();
    void init(TypesID id);
};