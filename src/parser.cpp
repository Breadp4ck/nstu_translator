#include "parser.hpp"
#include "table.hpp"
#include "token.hpp"
#include "types.hpp"
#include <cstdlib>

// РГЗ
// Методы диагностики и исправления ошибок.
// Ошибки, связанные с употреблением различных типов данных.

// -----------------------------------------------------------
// PARSE TABLE
// -----------------------------------------------------------

ParseTable::ParseTable(std::string filename) {
    std::ifstream file(filename);
    std::string line;

    std::getline(file, line);

    // Заполняем табличку из файла, строка за строкой
    while (std::getline(file, line)) {
        std::stringstream lineStream(line);

        std::vector<std::string> words;
        std::string word;

        while (std::getline(lineStream, word, '\t')) {
            words.push_back(word);
        }

        ParseTableRow row;

        std::stringstream terminalStream(words[1]);
        std::string terminal;

        while (std::getline(terminalStream, terminal, ' ')) {
            row.terminals_.push_back(terminal);
        }

        row.jump_ = std::stoi(words[2]);
        row.accept_ = std::stoi(words[3]);
        row.stack_ = std::stoi(words[4]);
        row.return_ = std::stoi(words[5]);
        row.error_ = std::stoi(words[6]);

        data.push_back(row);
    }
}

ParseTableRow& ParseTable::at(size_t idx) {
    // TODO: Index out of array exception
    return data[idx - 1];
}

void ParseTable::push(ParseTableRow row) {
    data.push_back(row);
}

void ParseTable::print() {
    for (auto& row : data) {

        for (auto& word : row.terminals_) {
            std::cout << word << " ";
        }

        std::cout << row.jump_ << " ";
        std::cout << row.accept_ << " ";
        std::cout << row.stack_ << " ";
        std::cout << row.return_ << " ";
        std::cout << row.error_ << std::endl;
    }
}

// -----------------------------------------------------------
// ADDITIONAL FUNCTIONS
// -----------------------------------------------------------

int getPriority(size_t op) {
    switch (op) {
        case 0:
        case 1:
            return 5;

        case 2:
            return 4;

        case 3:
            return 12;

        case 4:
        case 5:
            return 8;

        case 6:
        case 7:
            return 7;
        
    }

    return 199;
}

// -----------------------------------------------------------
// PARSER
// -----------------------------------------------------------

Parser::Parser(ParseTable *table, Tables tables, std::vector<size_t> tokenLineIndeces) {
    this->table = table;
    this->tables = tables;
    this->tokenLineIndeces = tokenLineIndeces;
}

std::string Parser::getTokenStr(Token token) {
    std::string tokenStr;

    switch (token.tableID) {
        case TABLE_BRACKETS:
            tokenStr = tables.brackets->GetValue(token.rowID);
            break;

        case TABLE_SPECIALS:
            tokenStr = tables.specials->GetValue(token.rowID);
            break;

        case TABLE_OPERATIONS:
            tokenStr = tables.operations->GetValue(token.rowID);
            break;

        case TABLE_TYPES:
            tokenStr = tables.types->GetValue(token.rowID);
            break;

        case TABLE_SEPARATORS:
            tokenStr = tables.separators->GetValue(token.rowID);
            break;

        case TABLE_VARIABLES:
            tokenStr = "var";
            break;

        case TABLE_CONSTANTS:
            tokenStr = "const";
            break;
    }

    return tokenStr;
}

std::string Parser::getNiceTokenStr(Token token) {
    std::string tokenStr;

    switch (token.tableID) {
        case TABLE_BRACKETS:
        case TABLE_SPECIALS:
        case TABLE_OPERATIONS:
        case TABLE_TYPES:
        case TABLE_SEPARATORS:
            tokenStr = getTokenStr(token);
            break;

        case TABLE_VARIABLES:
            tokenStr = tables.variables->ByHash(token.rowID).name();
            break;

        case TABLE_CONSTANTS:
            tokenStr = tables.constants->ByHash(token.rowID).getStr();
            break;
    }

    return tokenStr;

}

bool Parser::containsTerminal(std::vector<std::string> terminals, std::string terminal) {
    for (auto& t : terminals) {
        if (t.compare(terminal) == 0) {
            return true;
        }
    }

    return false;
}

void Parser::parse(std::vector<Token> tokens) {
    std::stack<size_t> st;
    std::vector<Token> output;
    std::stack<Token> outputBuf;
    size_t currentRow = 1;
    size_t currentTokenNumber = 0;
    size_t currentLine = 1;
    TypesID tempTypeID = TYPE_NULL;

    // For RGZ
    enum NumberType {
        NUMBER_INTEGER,
        NUMBER_FLOAT
    };

    NumberType rExpr = NUMBER_INTEGER;
    NumberType lExpr = NUMBER_INTEGER;

    outputBuf.push({100, 100});

    for (auto& token : tokens) {
        std::string tokenStr = getTokenStr(token);

        while (tokenLineIndeces[currentLine - 1] == currentTokenNumber) {
            currentLine++;
        }

        while (true) {
            ParseTableRow row = table->at(currentRow);
            size_t tempCurrentRow = currentRow; // Unreadable

            //std::cout << row.jump_ << " |\t" << currentRow << " &\t" << tokenStr;
            //getchar();

            // Choose temp type
            if (currentRow == 27)       tempTypeID = TYPE_INT;
            else if (currentRow == 28)  tempTypeID = TYPE_CHAR;
            else if (currentRow == 29)  tempTypeID = TYPE_FLOAT;
            else if (currentRow == 23)  tempTypeID = TYPE_NULL;

            if (row.stack_) {
                //std::cout << "- PUSH TO STACK " << currentRow << std::endl;
                st.push(currentRow);
            }
            
            if (row.return_) {
                //std::cout << " . RETURN TO " << st.top() << std::endl;
                currentRow = st.top() + 1;
                st.pop();

            } else if (row.jump_ != 0 && containsTerminal(row.terminals_, tokenStr)) {
                //std::cout << " . JUMP TO " << row.jump_ << std::endl;
                currentRow = row.jump_;

            } else if (!row.error_) {
                currentRow += 1;

            } else {
                ParserError err = { "Неизвестная ошибка", (int) currentLine }; // TODO to int? really?

                switch (currentRow) {
                    case 22:
                        err.msg = "Отсутствует имя переменной";
                        break;
                        
                    case 20:
                    case 71:
                        err.msg = "Неверный баланс скобок";
                        break;

                    case 31:
                        err.msg = "Пропущена точка с запятой";
                        break;

                    case 48:
                        err.msg = "Неправильная конструкция. Возможно, вы забыли '='?";
                        break;

                    case 64:
                        err.msg = "Опущен операнд";
                        break;

                    case 70:
                        err.msg = "В скобках должно быть хоть что-то";
                        break;

                    default:
                        break;
                }

                addError(err);
                return;
            }

            Type constant;
            Variable var("\0");

            if (row.accept_) {
                // Check existance of variable or constant in table
                switch (token.tableID) {
                    case TABLE_CONSTANTS:
                        constant = tables.constants->ByHash(token.rowID);

                        if (constant.type() == TYPE_FLOAT) {
                            rExpr = NUMBER_FLOAT;
                        }

                        break;

                    case TABLE_VARIABLES:
                        var = tables.variables->ByHash(token.rowID);

                        if (var.type() == TYPE_NULL) {
                            if (tempCurrentRow == 30) {
                                var.init(tempTypeID);
                                tables.variables->Update(var.name(), var);

                            } else {
                                ParserError err = { "Неопределённая переменная", (int) currentLine };
                                addError(err);
                                return;
                            }

                        } else {
                            if (tempCurrentRow == 30) {
                                ParserError err = { "Переопределённая переменная", (int) currentLine };
                                addError(err);
                                return;

                            }
                        }

                        if (var.type() == TYPE_FLOAT) {
                            if (tempCurrentRow == 30 || tempCurrentRow == 47) {
                                lExpr = NUMBER_FLOAT;

                            } else {
                                rExpr = NUMBER_FLOAT;
                            }
                        }

                        break;

                    case TABLE_SEPARATORS:
                        if (lExpr == NUMBER_INTEGER && rExpr == NUMBER_FLOAT) {
                            ParserError err = { "Невозможно приравнять вещественное число к целому", (int) currentLine };
                            addError(err);
                            return;
                        }

                        lExpr = NUMBER_INTEGER;
                        rExpr = NUMBER_INTEGER;

                        break;
                }

                // -------------------------------------------------------------
                //  Deals with stack
                // -------------------------------------------------------------
                if (token.tableID == TABLE_CONSTANTS
                        || token.tableID == TABLE_VARIABLES
                        || token.tableID == TABLE_SEPARATORS
                        || token.tableID == TABLE_OPERATIONS
                        || (token.tableID == TABLE_BRACKETS && currentRow > 9 && (token.rowID == 0 || token.rowID == 1))) {
                    
                    switch (token.tableID) {
                        case TABLE_CONSTANTS:
                        case TABLE_VARIABLES:
                            output.push_back(token);
                            break;

                        case TABLE_SEPARATORS:
                            while (outputBuf.top().tableID != 100) {
                                output.push_back(outputBuf.top());
                                outputBuf.pop();
                            }

                            output.push_back(token);
                            break;

                        case TABLE_BRACKETS:
                            if (token.rowID == 0) {
                                outputBuf.push(token);

                            } else if (token.rowID == 1) {
                                while ( ! (outputBuf.top().tableID == TABLE_BRACKETS && outputBuf.top().rowID == 0)) {
                                    output.push_back(outputBuf.top());
                                    outputBuf.pop();
                                }

                                outputBuf.pop();
                            }
                            break;

                        case TABLE_OPERATIONS:
                            size_t op = token.rowID;
                            Token last = outputBuf.top();

                            if (last.tableID == 100) {
                                outputBuf.push(token);

                            } else if (last.tableID == TABLE_OPERATIONS) {
                                if (getPriority(op) <= getPriority(outputBuf.top().rowID)) {
                                    outputBuf.push(token);

                                } else {
                                    output.push_back(last);
                                    outputBuf.pop();

                                    outputBuf.push(token);
                                }

                            } else {
                                outputBuf.push(token);
                            }
                            break;
                    }
                }

                break;
            }
        }

        // We count tokens with this variable. Don't forget!
        currentTokenNumber++;

        //std::cout << ">|\t" << tokenStr << std::endl;
    }

    if (st.empty()) {
        std::cout << "Всё круто" << std::endl;

    } else {
        ParserError err = { "Стек парсера не пустой. Возможно, вы забыли '}'?", (int) currentLine };
        addError(err);
        return;
    }

    // -----------------------------------------------------------
    // Кривой перевод в Речь Посполитую
    // -----------------------------------------------------------
    
    //std::stack<Token> mem;
    //std::stack<Token> res;

    //for (auto item : output) {
    //    switch (item.tableID) {
    //        case TABLE_BRACKETS:
    //            if (item.rowID == 0) {
    //                mem.push(item);

    //            } else {
    //                Token kek;
    //                do {
    //                    kek = mem.top();
    //                    res.push(kek);
    //                    mem.pop();
    //                } while (kek.tableID != TABLE_BRACKETS && kek.rowID != 1);
    //                res.pop();
    //            }
    //            break;

    //        case TABLE_SEPARATORS:
    //            Token kek;
    //            do {
    //                kek = mem.top();
    //                res.push(kek);
    //                mem.pop();
    //            } while (!mem.empty());
    //            res.push(item);
    //            break;

    //        case TABLE_VARIABLES:
    //        case TABLE_CONSTANTS:
    //            res.push(item);
    //            break;

    //        case TABLE_OPERATIONS:
    //            mem.push(item);
    //            break;

    //        default:
    //            break;
    //    }
    //}

    //while (!res.empty()) {
    //    mem.push(res.top());
    //    res.pop();
    //}

    for (auto token : output) {
        std::cout << getNiceTokenStr(token) << " ";
    }

    //std::cout << std::endl;
}

// Мне нужен токен для определения переменной/константы
// 
void Parser::addError(ParserError error) {
    errors.push_back(error);
}

std::vector<ParserError> Parser::getErrors() {
    return errors;
}