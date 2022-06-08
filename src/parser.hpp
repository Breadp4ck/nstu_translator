
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stack>

#include "token.hpp"
#include "table.hpp"

struct ParseTableRow {
    std::vector<std::string> terminals_;
    int jump_;
    bool accept_;
    bool stack_;
    bool return_;
    bool error_;
};

struct ParserError {
    std::string msg;
    int line;
};

class ParseTable {
private:
    std::vector<ParseTableRow> data;

public:
    ParseTable(std::string filename);
    void push(ParseTableRow row);
    ParseTableRow& at(size_t idx);
    void print();
};

class Parser {
private:
    ParseTable *table;
    Tables tables;
    std::vector<ParserError> errors;
    std::vector<size_t> tokenLineIndeces;

public:
    Parser(ParseTable *table, Tables tables, std::vector<size_t> tokenLineIndeces);
    void parse(std::vector<Token> tokens);
    std::string getTokenStr(Token);
    std::string getNiceTokenStr(Token);
    bool containsTerminal(std::vector<std::string> terminals, std::string terminal);
    void addError(ParserError error);
    std::vector<ParserError> getErrors();
};