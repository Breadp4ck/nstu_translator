#include <iostream>
#include <fstream>
#include <sstream>

#include "types.hpp"
#include "table.hpp"
#include "scanner.hpp"
#include "parser.hpp"
#include "code_generator.hpp"

int main() {

    // PREPARATIONS
    // ------------------------------------------------------------

    auto types = new std::string[] {"int", "float", "char"};
    auto operations = new std::string[] { "+", "-", "*", "=", "==", "!=", "<", ">" };
    auto separators = new std::string[] {",", ";"};
    auto brackets = new std::string[] {"(", ")", "{", "}"};
    auto specials = new std::string[] {"main", "return", "void"};

    auto typesTable = new StaticTable<std::string>(types, 3);
    auto operationsTable = new StaticTable<std::string>(operations, 8);
    auto separatorsTable = new StaticTable<std::string>(separators, 2);
    auto bracketsTable = new StaticTable<std::string>(brackets, 4);
    auto specialsTable = new StaticTable<std::string>(specials, 3);

    auto variablesTable = new DynamicTable<std::string, Variable>();
    auto constantsTable = new DynamicTable<std::string, Type>();

    Tables tables = {
        typesTable,
        operationsTable,
        separatorsTable,
        bracketsTable,
        specialsTable,
        variablesTable,
        constantsTable,
    };

    std::string content;
    std::ifstream file("test.cpp");

    if (file) {
        std::ostringstream ss;
        ss << file.rdbuf();
        content = ss.str();
    }

    file.close();


    // SCANNER
    // ------------------------------------------------------------

    auto scanner = Scanner(tables);
    scanner.scan(content);

    auto s_errors = scanner.getErrors();
    auto tokens = scanner.getTokens();
    auto tokenLineIndeces = scanner.getTokenLineIndeces();

    for (auto &err : s_errors) {  
        std::cout << err.line << "|\t" << err.msg << '\n';
    }

    if (s_errors.size() > 0) {
        return 1;
    }


    // PARSER
    // ------------------------------------------------------------

    auto table = ParseTable("reference/ParseTable.csv");
    auto parser = Parser(&table, tables, tokenLineIndeces);
    parser.parse(tokens);

    auto p_errors = parser.getErrors();
    auto polish = parser.getPolish();

    for (auto &err : p_errors) {  
        std::cout << err.line << "|\t" << err.msg << '\n';
    }

    if (p_errors.size() > 0) {
        return 1;
    }

    //for (auto token : polish) {
    //    std::cout << parser.getNiceTokenStr(token) << " ";
    //}


    // CODE GENERATOR
    // ------------------------------------------------------------

    auto gen = CodeGenerator(tables, polish);
    gen.generate("hello.asm");

    std::cout << "Done" << std::endl;

    return 0;
}