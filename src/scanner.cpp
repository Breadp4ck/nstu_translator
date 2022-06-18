#include "scanner.hpp"

// -----------------------------------------------------------
// SCANNER
// -----------------------------------------------------------

Scanner::Scanner(Tables tables) {
    tables_ = tables;
}

void Scanner::scan(std::string code) {
    transitionTo(ScannerStateStart());

    for (char lexeme : code) {
        state_.update(*this, lexeme);

        if (lexeme == '\n') {
            tokenLineIndeces.push_back(tokens.size());
            currentLine++;
        }
    }

    if (multilineComment) {
        pushError("Многострочный комментарий не закрыт.");
    }

    if (charContent) {
        pushError("Char-константа не закрыта.");
    }

    state_.update(*this, ' ');
}

void Scanner::transitionTo(ScannerState newState) {
    state_ = newState;
}

void Scanner::pushToBuffer(char lexeme) {
    buf.push_back(lexeme);
}

void Scanner::pushError(std::string errorMsg) {
    ScannerError err = { errorMsg, getLine() };
    errors.push_back(err);
}

int Scanner::getLine() {
    return currentLine;
}

std::vector<size_t> Scanner::getTokenLineIndeces() {
    return tokenLineIndeces;
}

void Scanner::clearBuffer() {
    buf.clear();
}

void Scanner::generateToken(TokenType type) {
    auto tokenStr = getBufferAsString();

    switch (type) {
        case TokenOperation:
            if (tables_.operations->Contains(tokenStr)) {
                Token token = { TABLE_OPERATIONS, tables_.operations->GetID(tokenStr) };
                tokens.push_back(token);

            } else {
                pushError("Задана неправильная операция.");
            }
            break;

        case TokenSeparator:
            if (tables_.separators->Contains(tokenStr)) {
                Token token = { TABLE_SEPARATORS, tables_.separators->GetID(tokenStr) };
                tokens.push_back(token);

            } else {
                pushError("Задан неправильный разделитель.");
            }
            break;

        case TokenWord:
            if (tables_.types->Contains(tokenStr)) {
                Token token = { TABLE_TYPES, tables_.types->GetID(tokenStr) };
                tokens.push_back(token);

            } else if (tables_.specials->Contains(tokenStr)) {
                Token token = { TABLE_SPECIALS, tables_.specials->GetID(tokenStr) };
                tokens.push_back(token);

            } else {
                tables_.variables->Update(tokenStr, Variable(tokenStr));
                Token token = { TABLE_VARIABLES, tables_.variables->getHash(tokenStr) };
                tokens.push_back(token);
            }
            break;

        case TokenConstant:
            int valueInt;
            float valueFloat;

            if (Int::TryParse(tokenStr, &valueInt)) {
                tables_.constants->Update(tokenStr, Int(valueInt));
                Token token = { TABLE_CONSTANTS, tables_.constants->getHash(tokenStr) };
                tokens.push_back(token);

            } else if (Float::TryParse(tokenStr, &valueFloat)) {
                tables_.constants->Update(tokenStr, Float(valueFloat));
                Token token = { TABLE_CONSTANTS, tables_.constants->getHash(tokenStr) };
                tokens.push_back(token);

            } else {
                pushError("Задана неправильная числовая константа.");
            }

            break;

        case TokenConstantChar:
            char value;

            if (Char::TryParse(tokenStr, &value)) {
                tables_.constants->Update(tokenStr, Char(value));
                Token token = { TABLE_CONSTANTS, tables_.constants->getHash(tokenStr) };
                tokens.push_back(token);

            } else {
                pushError("Задана неправильная символьная константа.");
            }
            break;

        case TokenBracket:
            if (tables_.brackets->Contains(tokenStr)) {
                Token token = { TABLE_BRACKETS, tables_.brackets->GetID(tokenStr) };
                tokens.push_back(token);
            }
            break;

        case TokenError:
            break;
    }

    clearBuffer();
}

std::string Scanner::getBufferAsString() {
    std::string str(buf.begin(), buf.end());
    return str;
}

std::vector<ScannerError> Scanner::getErrors() {
    return errors;
}

std::vector<Token> Scanner::getTokens() {
    return tokens;
}


// -----------------------------------------------------------
// STATES
// -----------------------------------------------------------

void ScannerStateStart::update(Scanner& scanner, char lexeme) {
    if (isalpha(lexeme) || lexeme == '_') {
        scanner.transitionTo(ScannerStateWord());
        scanner.pushToBuffer(lexeme);

    } else if (isdigit(lexeme)) {
        scanner.transitionTo(ScannerStateNumber());
        scanner.pushToBuffer(lexeme);

    } else if (lexeme == '-' || lexeme == '+' || lexeme == '*' || lexeme == '=' || lexeme == '!' || lexeme == '<' || lexeme == '>') {
        scanner.transitionTo(ScannerStateOperation());
        scanner.pushToBuffer(lexeme);

    } else if (lexeme == '/') {
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(ScannerStateCommentOrOperation());

    } else if (lexeme == ';' || lexeme == ',') {
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenSeparator);

    } else if (lexeme == '{' || lexeme == '}' || lexeme == '(' || lexeme == ')') {
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenBracket);

    } else if (isspace(lexeme)) {
        scanner.clearBuffer();

    } else if (lexeme == '\'') {
        scanner.charContent = true;
        scanner.transitionTo(ScannerStateCharContent());
    }
}

void ScannerStateWord::update(Scanner& scanner, char lexeme) {
    if (isalpha(lexeme) || isdigit(lexeme) || lexeme == '_') {
        scanner.pushToBuffer(lexeme);

    } else if (isspace(lexeme)) {
        scanner.generateToken(TokenWord);
        scanner.transitionTo(ScannerStateStart());

    } else if (lexeme == '-' || lexeme == '+' || lexeme == '*' || lexeme == '=' || lexeme == '!' || lexeme == '<' || lexeme == '>') {
        scanner.generateToken(TokenWord);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(ScannerStateOperation());

    } else if (lexeme == '/') {
        scanner.generateToken(TokenWord);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(ScannerStateCommentOrOperation());

    } else if (lexeme == ';' || lexeme == ',') {
        scanner.generateToken(TokenWord);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenSeparator);
        scanner.transitionTo(ScannerStateStart());

    } else if (lexeme == '{' || lexeme == '}' || lexeme == '(' || lexeme == ')') {
        scanner.generateToken(TokenWord);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenBracket);
        scanner.transitionTo(ScannerStateStart());

    } else if (lexeme == '\'') {
        scanner.charContent = true;
        scanner.transitionTo(ScannerStateCharContent());
    }
}

void ScannerStateNumber::update(Scanner& scanner, char lexeme) {
    if (isdigit(lexeme)) {
        scanner.pushToBuffer(lexeme);

    } else if (lexeme == '.') {
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(ScannerStateFloatNumber());

    } else if (isspace(lexeme)) {
        scanner.generateToken(TokenConstant);
        scanner.transitionTo(ScannerStateStart());

    } else if (lexeme == '-' || lexeme == '+' || lexeme == '*' || lexeme == '=' || lexeme == '!' || lexeme == '<' || lexeme == '>') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(ScannerStateOperation());

    } else if (lexeme == '/') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(ScannerStateCommentOrOperation());

    } else if (lexeme == ';' || lexeme == ',') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenSeparator);
        scanner.transitionTo(ScannerStateStart());

    } else if (lexeme == '{' || lexeme == '}' || lexeme == '(' || lexeme == ')') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenBracket);
        scanner.transitionTo(ScannerStateStart());

    } else if (isalpha(lexeme)) {
        scanner.pushError("Название переменной не может начинаться с цифры.");
        scanner.generateToken(TokenConstant);
        scanner.transitionTo(ScannerStateStart());

    } else if (lexeme == '\'') {
        scanner.charContent = true;
        scanner.transitionTo(ScannerStateCharContent());
    }
}

void ScannerStateFloatNumber::update(Scanner& scanner, char lexeme) {
    if (isdigit(lexeme)) {
        scanner.pushToBuffer(lexeme);

    } else if (lexeme == '.') {
        scanner.pushError("У константы Float может быть только одна точка.");
        scanner.generateToken(TokenError);
        scanner.transitionTo(ScannerStateStart());

    } else if (isspace(lexeme)) {
        scanner.generateToken(TokenConstant);
        scanner.transitionTo(ScannerStateStart());

    } else if (lexeme == '-' || lexeme == '+' || lexeme == '*' || lexeme == '=' || lexeme == '!' || lexeme == '<' || lexeme == '>') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(ScannerStateOperation());

    } else if (lexeme == '/') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(ScannerStateCommentOrOperation());

    } else if (lexeme == ';' || lexeme == ',') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenSeparator);
        scanner.transitionTo(ScannerStateStart());

    } else if (lexeme == '{' || lexeme == '}' || lexeme == '(' || lexeme == ')') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenBracket);
        scanner.transitionTo(ScannerStateStart());

    } else if (isalpha(lexeme)) {
        scanner.pushError("Числовая константа не может содержать букв.");
        scanner.generateToken(TokenConstant);
        scanner.transitionTo(ScannerStateStart());

    } else if (lexeme == '\'') {
        scanner.charContent = true;
        scanner.transitionTo(ScannerStateCharContent());
    }
}

void ScannerStateOperation::update(Scanner& scanner, char lexeme) {
    if (lexeme == '/' || lexeme == '-' || lexeme == '+' || lexeme == '*' || lexeme == '=' || lexeme == '!' || lexeme == '<' || lexeme == '>') {
        scanner.pushToBuffer(lexeme);

    } else if (isspace(lexeme)) {
        scanner.generateToken(TokenOperation);
        scanner.transitionTo(ScannerStateStart());

    } else if (isdigit(lexeme)) {
        scanner.generateToken(TokenOperation);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(ScannerStateNumber());

    } else if (isalpha(lexeme)) {
        scanner.generateToken(TokenOperation);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(ScannerStateWord());

    } else if (lexeme == ';' || lexeme == ',') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenSeparator);
        scanner.transitionTo(ScannerStateStart());

    } else if (lexeme == '{' || lexeme == '}' || lexeme == '(' || lexeme == ')') {
        scanner.generateToken(TokenWord);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenBracket);
        scanner.transitionTo(ScannerStateStart());

    } else if (lexeme == '\'') {
        scanner.charContent = true;
        scanner.transitionTo(ScannerStateCharContent());
    }
}

void ScannerStateCommentOrOperation::update(Scanner& scanner, char lexeme) {
    if (lexeme == '-' || lexeme == '+' || lexeme == '*' || lexeme == '=' || lexeme == '!' || lexeme == '<' || lexeme == '>') {
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(ScannerStateOperation());

    } else if (isspace(lexeme)) {
        scanner.generateToken(TokenOperation);
        scanner.transitionTo(ScannerStateStart());

    } else if (isdigit(lexeme)) {
        scanner.generateToken(TokenOperation);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(ScannerStateNumber());

    } else if (isalpha(lexeme)) {
        scanner.generateToken(TokenOperation);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(ScannerStateWord());

    } else if (lexeme == '/') {
        scanner.clearBuffer();
        scanner.transitionTo(ScannerStateSinglelineComment());

    } else if (lexeme == '*') {
        scanner.multilineComment = true;
        scanner.clearBuffer();
        scanner.transitionTo(ScannerStateMultilineComment());

    } else if (lexeme == ';' || lexeme == ',') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenSeparator);
        scanner.transitionTo(ScannerStateStart());

    } else if (lexeme == '{' || lexeme == '}' || lexeme == '(' || lexeme == ')') {
        scanner.generateToken(TokenWord);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenBracket);
        scanner.transitionTo(ScannerStateStart());

    } else if (lexeme == '\'') {
        scanner.charContent = true;
        scanner.transitionTo(ScannerStateCharContent());
    }
}

void ScannerStateSinglelineComment::update(Scanner& scanner, char lexeme) {
    if (lexeme == '\n') {
        scanner.transitionTo(ScannerStateStart());
    }
}

void ScannerStateMultilineComment::update(Scanner& scanner, char lexeme) {
    if (lexeme == '*') {
        scanner.transitionTo(ScannerStateFinishMultilineComment());
    }
}

void ScannerStateFinishMultilineComment::update(Scanner& scanner, char lexeme) {
    if (lexeme == '/') {
        scanner.multilineComment = false;
        scanner.transitionTo(ScannerStateStart());
    }
}

void ScannerStateCharContent::update(Scanner& scanner, char lexeme) {
    if (lexeme == '\'') {
        scanner.charContent = false;
        scanner.generateToken(TokenConstantChar);
        scanner.transitionTo(ScannerStateStart());

    } else {
        scanner.pushToBuffer(lexeme);
    }
}

void ScannerStateCharEscape::update(Scanner& scanner, char lexeme) {
    scanner.pushToBuffer(lexeme);
    scanner.transitionTo(ScannerStateCharContent());
}