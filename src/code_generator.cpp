#include "code_generator.hpp"

#include <iostream>
#include <string>

#include "types.hpp"
#include "table.hpp"
#include "variable.hpp"


struct Number {
    std::string name;
    size_t typeID;
};


// -----------------------------------------------------------
// ASSEMBLER_GENERATOR
// -----------------------------------------------------------

std::string addParser(Number p1, Number p2, Number r) {
    std::string result = "";

    result += p1.typeID == TYPE_FLOAT ? "\tfld qword [" : "\tfild qword [";
    result += p1.name;
    result += p2.typeID == TYPE_FLOAT ? "]\n\tfadd qword [" : "]\n\tfiadd qword [";
    result += p2.name;
    result += r.typeID == TYPE_FLOAT ? "]\n\tfstp qword [" : "]\n\tfistp qword [";
    result += r.name;
    result += "]\n\n";

    return result;
}

std::string subParser(Number p1, Number p2, Number r) {
    std::string result = "";

    result += p1.typeID == TYPE_FLOAT ? "\tfld qword [" : "\tfild qword [";
    result += p1.name;
    result += p2.typeID == TYPE_FLOAT ? "]\n\tfsub qword [" : "]\n\tfisub qword [";
    result += p2.name;
    result += r.typeID == TYPE_FLOAT ? "]\n\tfstp qword [" : "]\n\tfistp qword [";
    result += r.name;
    result += "]\n\n";

    return result;
}

std::string mulParser(Number p1, Number p2, Number r) {
    std::string result = "";

    result += p1.typeID == TYPE_FLOAT ? "\tfld qword [" : "\tfild qword [";
    result += p1.name;
    result += p2.typeID == TYPE_FLOAT ? "]\n\tfmul qword [" : "]\n\tfimul qword [";
    result += p2.name;
    result += r.typeID == TYPE_FLOAT ? "]\n\tfstp qword [" : "]\n\tfistp qword [";
    result += r.name;
    result += "]\n\n";

    return result;
}

std::string setParser(Number p1, Number _p2, Number r) {
    std::string result = "";

    result += r.typeID == TYPE_FLOAT ? "\tfld qword [" : "\tfild qword [";
    result += r.name;
    result += p1.typeID == TYPE_FLOAT ? "]\n\tfstp qword [" : "]\n\tfistp qword [";
    result += p1.name;
    result += "]\n\n";

    return result;
}

// -----------------------------------------------------------
// ADDITIONAL FUNCTIONS
// -----------------------------------------------------------

std::string getSalt(std::string prefix, int some) {
    std::string salt = prefix + std::to_string(some);

    return salt;
}

size_t CodeGenerator::getType(Token token) {
    switch (token.tableID) {
        case TABLE_CONSTANTS:
            return tables.constants->ByHash(token.rowID).type();

        case TABLE_VARIABLES:
            return tables.variables->ByHash(token.rowID).type();

        case TABLE_TEMPS:
            return tempsTable.Get(token.rowID).type();
    }

    return 100; // TODO oh my, this is error
}

std::string CodeGenerator::getNiceName(Token token) {
    switch (token.tableID) {
        case TABLE_CONSTANTS:
            return getSalt("CONST_", token.rowID);

        case TABLE_VARIABLES:
            return tables.variables->ByHash(token.rowID).name();

        case TABLE_TEMPS:
            return getSalt("TEMP_", token.rowID);;
    }

    return "kek"; // TODO oh my, this is error
}

// -----------------------------------------------------------
// CODE GENERATOR
// -----------------------------------------------------------

CodeGenerator::CodeGenerator(Tables tables, std::vector<Token> polish) {
    this->tables = tables;
    this->polish = polish;

    this->tempsTable = DynamicTable<size_t, Variable>();
}

std::string CodeGenerator::generate() {
    std::vector<Token> operandStack; // smth like buffer to parse to 4s ("четвёрки")
    std::vector<Token> tempVars; // additional variables to store data

    // TODO It's really bad, but I don't have enough time

    // TODO When I'll consume what is it, I write smth more definitive.
    size_t idx1 = 0, idx2 = 0;
    std::string salt;

    // TODO I need to figure out how to write all constants and variables (temp included)
    std::cout << "[section .text]" << std::endl;
    std::cout << "\tglobal _start\n\n_start:" << std::endl;
    for (auto token : polish) {
        switch (token.tableID) {
            case TABLE_VARIABLES:
            case TABLE_CONSTANTS:
                operandStack.push_back(token);
                idx1++;
                break;

            case TABLE_OPERATIONS:
                Token tokenP2 = operandStack.back(); 
                operandStack.pop_back(); 
                Token tokenP1 = operandStack.back(); 
                operandStack.pop_back(); 

                size_t typeP1 = getType(tokenP1);
                size_t typeP2 = getType(tokenP2);

                size_t minTemp = idx2;
                Token tokenT = tokenP1;

                // TODO At the beginning I'll create as many temp variables as possible
                if (tokenP1.tableID == TABLE_TEMPS) {
                    minTemp = tokenP1.rowID;                    
                    idx2 = minTemp; 
                }

                if (tokenP2.tableID == TABLE_TEMPS && tokenP2.rowID < minTemp) {
                    minTemp = tokenP2.rowID;
                    idx2 = minTemp; 
                    tokenT = tokenP2;
                }

                std::string nameP1 = getNiceName(tokenP1);
                std::string nameP2 = getNiceName(tokenP2);
                std::string nameT = getNiceName(tokenT);

                if (minTemp == idx2) {
                    salt = getSalt("TEMP_", idx2);
                    std::string tempName = std::to_string(idx2);
                    Variable temp(tempName);

                    // Init with type
                    if (typeP1 == TYPE_FLOAT || typeP2 == TYPE_FLOAT) {
                        temp.init(TYPE_FLOAT);

                    } else if (typeP1 == TYPE_INT || typeP2 == TYPE_INT) {
                        temp.init(TYPE_INT);

                    } else {
                        temp.init(TYPE_CHAR);
                    }

                    tempsTable.Update(idx2, temp);
                    tokenT = { TABLE_TEMPS, idx2 }; // Special case to work

                    nameT = getNiceName(tokenT);
                    operandStack.push_back(tokenT);
                    idx2++; 
                }

                Number p1 = { nameP1, typeP1 };
                Number p2 = { nameP2, typeP2 };
                Number t = { nameT, getType(tokenT) };


                // Depends on operation, I'll generate assembler code
                switch (token.rowID) {
                    case 0: // +
                        std::cout << "\t; " << nameP1 << " + " << nameP2 << " -> " << nameT << '\n'
                                  << addParser(p1, p2, t);
                        break;

                    case 1: // -
                        std::cout << "\t; " << nameP1 << " - " << nameP2 << " -> " << nameT << '\n'
                                  << subParser(p1, p2, t);
                        break;

                    case 2: // *
                        std::cout << "\t; " << nameP1 << " * " << nameP2 << " -> " << nameT << '\n'
                                  << mulParser(p1, p2, t);
                        break;

                    case 3: // =
                        std::cout << "\t; " << nameP1 << " = " << nameP2 << " -> " << nameP1 << '\n'
                                  << setParser(p1, p2, t);
                        break;

                    case 4: // ==
                        std::cout << "== " << nameP1 << " " << nameP2 << " " << nameT << std::endl;
                        break;

                    case 5: // !=
                        std::cout << "!= " << nameP1 << " " << nameP2 << " " << nameT << std::endl;
                        break;

                    case 6: // <
                        std::cout << "< " << nameP1 << " " << nameP2 << " " << nameT << std::endl;
                        break;

                    case 7: // >
                        std::cout << "> " << nameP1 << " " << nameP2 << " " << nameT << std::endl;
                        break;
                }

                // If I set idx2 = minTemp, then algorithm will not work,
                // but invalid pointer error will be vanished

                break;
        }
    }

    // DATA SECTION
    // -----------------------------------------------------------
    std::cout << "\t; Finish is here\n\tmov eax, 1\n\tint 0x80\n\n";

    std::cout << "\n[section .data]" << std::endl;
    std::stack<Token> allNames;

    for (auto temp : tempsTable.getKeys()) {
        allNames.push({ TABLE_TEMPS, tempsTable.getHash(temp) });
    }

    for (auto var : tables.variables->getKeys()) {
        allNames.push({ TABLE_VARIABLES, tables.variables->getHash(var) });
    }

    while (!allNames.empty()) {
        auto token = allNames.top();
        auto type = getType(token);

        switch (type) {
            case TYPE_INT:
                std::cout << "\t" << getNiceName(token) << " dd 0" << std::endl;
                break;

            case TYPE_CHAR:
                std::cout << "\t" << getNiceName(token) << " db 0" << std::endl;
                break;

            case TYPE_FLOAT:
                std::cout << "\t" << getNiceName(token) << " dq 0.0" << std::endl;
                break;
        }

        allNames.pop();
    }

    for (auto constant : tables.constants->getKeys()) {
        allNames.push({ TABLE_CONSTANTS, tables.constants->getHash(constant) });
    }

    while (!allNames.empty()) {
        auto token = allNames.top();
        auto tokenStr = tables.constants->ByHash(token.rowID).getStr();
        auto type = getType(token);

        switch (type) {
            case TYPE_INT:
                std::cout << "\t" << getNiceName(token) << " dd " << tokenStr << std::endl;
                break;

            case TYPE_CHAR:
                std::cout << "\t" << getNiceName(token) << " db " << tokenStr << std::endl;
                break;

            case TYPE_FLOAT:
                std::cout << "\t" << getNiceName(token) << " dq " << tokenStr << std::endl;
                break;
        }

        allNames.pop();
    }

    return "";
}