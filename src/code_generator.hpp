#pragma once

#include <string>

#include "token.hpp"
#include "table.hpp"

// TODO AAAAAAA THIS IS UNPROPRIATE!!!
const size_t TABLE_TEMPS = 20; 

class CodeGenerator {
    Tables tables;
    std::vector<Token> polish;
    DynamicTable<size_t, Variable> tempsTable;

    public:
        CodeGenerator(Tables tables, std::vector<Token> polish);
        size_t getType(Token token);
        std::string getNiceName(Token token);
        std::string generate(std::string filename);
};