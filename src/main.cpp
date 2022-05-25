#include <iostream>
#include <fstream>
#include <sstream>

#include "scanner.hpp"
#include "parser.hpp"

int main() {
    std::string content;
    std::ifstream file("test.cpp");

    if (file) {
        std::ostringstream ss;
        ss << file.rdbuf();
        content = ss.str();
    }

    file.close();

    auto scanner = Scanner();
    scanner.scan(content);
    auto errors = scanner.getErrors();

    for (auto &err : errors) {  
        std::cout << err.line << "|\t" << err.msg << '\n';
    }


    return 0;
}