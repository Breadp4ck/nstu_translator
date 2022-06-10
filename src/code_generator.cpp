#include "code_generator.hpp"

#include <iostream>
#include <fstream>
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

    result += p1.typeID == TYPE_FLOAT ? "\tfld qword [" : "\tfild dword [";
    result += p1.name;
    result += p2.typeID == TYPE_FLOAT ? "]\n\tfadd qword [" : "]\n\tfiadd dword [";
    result += p2.name;
    result += r.typeID == TYPE_FLOAT ? "]\n\tfstp qword [" : "]\n\tfistp dword [";
    result += r.name;
    result += "]\n\n";

    return result;
}

std::string subParser(Number p1, Number p2, Number r) {
    std::string result = "";

    result += p1.typeID == TYPE_FLOAT ? "\tfld qword [" : "\tfild dword [";
    result += p1.name;
    result += p2.typeID == TYPE_FLOAT ? "]\n\tfsub qword [" : "]\n\tfisub dword [";
    result += p2.name;
    result += r.typeID == TYPE_FLOAT ? "]\n\tfstp qword [" : "]\n\tfistp dword [";
    result += r.name;
    result += "]\n\n";

    return result;
}

std::string mulParser(Number p1, Number p2, Number r) {
    std::string result = "";

    result += p1.typeID == TYPE_FLOAT ? "\tfld qword [" : "\tfild dword [";
    result += p1.name;
    result += p2.typeID == TYPE_FLOAT ? "]\n\tfmul qword [" : "]\n\tfimul dword [";
    result += p2.name;
    result += r.typeID == TYPE_FLOAT ? "]\n\tfstp qword [" : "]\n\tfistp dword [";
    result += r.name;
    result += "]\n\n";

    return result;
}

std::string setParser(Number p1, Number p2, Number r) {
    std::string result = "";

    result += p2.typeID == TYPE_FLOAT ? "\tfld qword [" : "\tfild dword [";
    result += p2.name;
    result += r.typeID == TYPE_FLOAT ? "]\n\tfstp qword [" : "]\n\tfistp dword [";
    result += r.name;
    result += "]\n";

    result += r.typeID == TYPE_FLOAT ? "\tfld qword [" : "\tfild dword [";
    result += r.name;
    result += p1.typeID == TYPE_FLOAT ? "]\n\tfstp qword [" : "]\n\tfistp dword [";
    result += p1.name;
    result += "]\n\n";

    return result;
}

std::string compParser(Number p1, Number p2, Number r, std::string jumpCond, size_t opIdx) {
    std::string result = "";

    std::string lbl_true = "lbl_";
    lbl_true += std::to_string(opIdx);
    lbl_true += "_";
    lbl_true += jumpCond;
    lbl_true += "_true";

    std::string lbl_exit = "lbl_";
    lbl_exit += std::to_string(opIdx);
    lbl_exit += "_";
    lbl_exit += jumpCond;
    lbl_exit += "_exit";

/*
    fld qword [p1]
    fcomp qword [p2]
    fstsw ax ; Take FPU compare flags
    sahw
    jXX lbl_IDX_eq_true
    fldz
    jmp lbl_IDX_eq_exit
lbl_IDX_eq_true: 
    fld1
lbl_IDX_eq_exit:
    fstp qword [r]
*/

    result += "\t; Comparation work only whith floats!\n";
    result += "\tfld qword [";
    result += p1.name;
    result += "]\n\tfld qword [";
    result += p2.name;
    result += "]\n\tfcomip\n";
    //result += "\tfstsw ax\n\tsahw\n\t";

    result += jumpCond;
    result += " ";
    result += lbl_true;

    result += "\n\tfldz\n\tjmp ";
    result += lbl_exit;
    result += "\n";

    result += lbl_true;
    result += ":\n\tfld1\n";

    result += lbl_exit;
    result += ":\n\tfistp dword [";

    result += r.name;
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

std::string CodeGenerator::generate(std::string filename) {
    std::vector<Token> operandStack; // smth like buffer to parse to 4s ("четвёрки")
    std::vector<Token> tempVars; // additional variables to store data

    std::ofstream file(filename); // output file

    // TODO It's really bad, but I don't have enough time

    // TODO When I'll consume what is it, I write smth more definitive.
    size_t idx1 = 0, idx2 = 0;
    size_t opIdx = 0;
    std::string salt;

    // TODO I need to figure out how to write all constants and variables (temp included)
    file << "; Hello World C compiler\n";
    file << "; Task 6, by Begichev and Shishkin\n\n";
    file << "; assemble:              nasm -f elf -l hello.lst hello.asm\n";
    file << "; assemble with debug:   nasm -g -f elf -l hello.lst hello.asm\n";
    file << "; link:                  gcc -m32 hello.o -o hello\n";
    file << "; run:                   hello\n\n";
    file << "[section .text]\n";
    file << "\tglobal main\n\nmain:\n";

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
                    size_t typeT = getType(tokenT);

                    if ( ! ((typeT == TYPE_INT || typeT == TYPE_CHAR) && (typeP2 == TYPE_FLOAT))) {
                        minTemp = tokenP1.rowID;                    
                        idx2 = minTemp; 
                    }
                }

                if (tokenP2.tableID == TABLE_TEMPS && tokenP2.rowID < minTemp) {
                    size_t typeT = getType(tokenP2);

                    if ( ! ((typeT == TYPE_INT || typeT == TYPE_CHAR) && (typeP1 == TYPE_FLOAT))) {
                        minTemp = tokenP2.rowID;
                        idx2 = minTemp; 
                        tokenT = tokenP2;
                    }
                }

                std::string nameP1 = getNiceName(tokenP1);
                std::string nameP2 = getNiceName(tokenP2);
                std::string nameT = getNiceName(tokenT);

                if (minTemp == idx2 && !tempsTable.Contains(idx2)) {
                    salt = getSalt("TEMP_", idx2);
                    std::string tempName = std::to_string(idx2);
                    Variable temp(tempName);


                    // Init with type

                    if (token.rowID < 4) {
                        if (typeP1 == TYPE_FLOAT || typeP2 == TYPE_FLOAT) {
                            temp.init(TYPE_FLOAT);

                        } else if (typeP1 == TYPE_INT || typeP2 == TYPE_INT) {
                            temp.init(TYPE_INT);

                        } else {
                            temp.init(TYPE_CHAR);
                        }

                    } else {
                        temp.init(TYPE_INT);
                    }

                    tempsTable.Update(idx2, temp);
                    tokenT = { TABLE_TEMPS, idx2 }; // Special case to work

                    nameT = getNiceName(tokenT);
                }

                idx2++; 

                Number p1 = { nameP1, typeP1 };
                Number p2 = { nameP2, typeP2 };
                Number t = { nameT, getType(tokenT) };

                operandStack.push_back(tokenT);

                // Depends on operation, I'll generate assembler code
                switch (token.rowID) {

                    case 0: // +
                        file << "\t; " << nameP1 << " + " << nameP2 << " -> " << nameT << '\n'
                                  << addParser(p1, p2, t);
                        break;

                    case 1: // -
                        file << "\t; " << nameP1 << " - " << nameP2 << " -> " << nameT << '\n'
                                  << subParser(p1, p2, t);
                        break;

                    case 2: // *
                        file << "\t; " << nameP1 << " * " << nameP2 << " -> " << nameT << '\n'
                                  << mulParser(p1, p2, t);
                        break;

                    case 3: // =
                        file << "\t; " << nameP1 << " = " << nameP2 << " -> " << nameP1 << '\n'
                                  << setParser(p1, p2, t);
                        break;

                    case 4: // ==
                        file << "\t; " << nameP1 << " == " << nameP2 << " -> " << nameT << '\n'
                                  << compParser(p1, p2, t, "je", opIdx);
                        break;

                    case 5: // !=
                        file << "\t; " << nameP1 << " != " << nameP2 << " -> " << nameT << '\n'
                                  << compParser(p1, p2, t, "jne", opIdx);
                        break;

                    case 6: // <
                        file << "\t; " << nameP1 << " < " << nameP2 << " -> " << nameT << '\n'
                                  << compParser(p1, p2, t, "ja", opIdx);
                        break;

                    case 7: // >
                        file << "\t; " << nameP1 << " > " << nameP2 << " -> " << nameT << '\n'
                                  << compParser(p1, p2, t, "jb", opIdx);
                        break;
                }

                // If I set idx2 = minTemp, then algorithm will not work,
                // but invalid pointer error will be vanished

                opIdx++;

                break;
        }
    }

    // DATA SECTION
    // -----------------------------------------------------------
    file << "\t; Exit program\n\tmov eax, 1\n\tint 80h\n\n";

    file << "\n[section .data]\n";
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
                file << "\t" << getNiceName(token) << " dd 0" << std::endl;
                break;

            case TYPE_CHAR:
                file << "\t" << getNiceName(token) << " db 0" << std::endl;
                break;

            case TYPE_FLOAT:
                file << "\t" << getNiceName(token) << " dq 0.0" << std::endl;
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
                file << "\t" << getNiceName(token) << " dd " << tokenStr << std::endl;
                break;

            case TYPE_CHAR:
                file << "\t" << getNiceName(token) << " db " << tokenStr << std::endl;
                break;

            case TYPE_FLOAT:
                file << "\t" << getNiceName(token) << " dq " << tokenStr << std::endl;
                break;
        }

        allNames.pop();
    }

    return "";
}