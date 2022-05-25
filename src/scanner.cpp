#include <typeinfo>
#include <iostream>
#include <string>

#include "scanner.hpp"

// -----------------------------------------------------------
// SCANNER
// -----------------------------------------------------------

void Scanner::scan(std::string code) {
    transitionTo(new ScannerStateStart());

    for (char lexeme : code) {
        state_->update(*this, lexeme);

        if (lexeme == '\n') {
            currentLine++;
        }
    }

    if (multilineComment) {
        pushError("Многострочный комментарий не закрыт.");
    }

    if (charContent) {
        pushError("Char-константа не закрыта.");
    }

    state_->update(*this, ' ');
}

void Scanner::transitionTo(ScannerState *newState) {
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

void Scanner::clearBuffer() {
    buf.clear();
}

void Scanner::generateToken(TokenType type) {
    auto tokenStr = getBufferAsString();
    std::cout << tokenStr << std::endl;

    switch (type) {
        case TokenOperation:
            break;

        case TokenSeparator:
            break;

        case TokenWord:
            break;

        case TokenConstant:
            break;

        case TokenConstantChar:
            break;

        case TokenBracket:
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


// -----------------------------------------------------------
// STATES
// -----------------------------------------------------------

void ScannerStateStart::update(Scanner& scanner, char lexeme) {
    if (isalpha(lexeme) || lexeme == '_') {
        scanner.transitionTo(new ScannerStateWord());
        scanner.pushToBuffer(lexeme);

    } else if (isdigit(lexeme)) {
        scanner.transitionTo(new ScannerStateNumber());
        scanner.pushToBuffer(lexeme);

    } else if (lexeme == '-' || lexeme == '+' || lexeme == '*' || lexeme == '=') {
        scanner.transitionTo(new ScannerStateOperation());
        scanner.pushToBuffer(lexeme);

    } else if (lexeme == '/') {
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(new ScannerStateCommentOrOperation());

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
        scanner.transitionTo(new ScannerStateCharContent());
    }
}

void ScannerStateWord::update(Scanner& scanner, char lexeme) {
    if (isalpha(lexeme) || isdigit(lexeme) || lexeme == '_') {
        scanner.pushToBuffer(lexeme);

    } else if (isspace(lexeme)) {
        scanner.generateToken(TokenWord);
        scanner.transitionTo(new ScannerStateStart());

    } else if (lexeme == '-' || lexeme == '+' || lexeme == '*' || lexeme == '=') {
        scanner.generateToken(TokenWord);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(new ScannerStateOperation());

    } else if (lexeme == '/') {
        scanner.generateToken(TokenWord);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(new ScannerStateCommentOrOperation());

    } else if (lexeme == ';' || lexeme == ',') {
        scanner.generateToken(TokenWord);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenSeparator);
        scanner.transitionTo(new ScannerStateStart());

    } else if (lexeme == '{' || lexeme == '}' || lexeme == '(' || lexeme == ')') {
        scanner.generateToken(TokenWord);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenBracket);
        scanner.transitionTo(new ScannerStateStart());

    } else if (lexeme == '\'') {
        scanner.charContent = true;
        scanner.transitionTo(new ScannerStateCharContent());
    }
}

void ScannerStateNumber::update(Scanner& scanner, char lexeme) {
    if (isdigit(lexeme)) {
        scanner.pushToBuffer(lexeme);

    } else if (lexeme == '.') {
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(new ScannerStateFloatNumber());

    } else if (isspace(lexeme)) {
        scanner.generateToken(TokenConstant);
        scanner.transitionTo(new ScannerStateStart());

    } else if (lexeme == '-' || lexeme == '+' || lexeme == '*' || lexeme == '=') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(new ScannerStateOperation());

    } else if (lexeme == '/') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(new ScannerStateCommentOrOperation());

    } else if (lexeme == ';' || lexeme == ',') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenSeparator);
        scanner.transitionTo(new ScannerStateStart());

    } else if (lexeme == '{' || lexeme == '}' || lexeme == '(' || lexeme == ')') {
        scanner.generateToken(TokenWord);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenBracket);
        scanner.transitionTo(new ScannerStateStart());

    } else if (isalpha(lexeme)) {
        scanner.pushError("Название переменной не может начинаться с цифры.");
        scanner.generateToken(TokenConstant);
        scanner.transitionTo(new ScannerStateStart());

    } else if (lexeme == '\'') {
        scanner.charContent = true;
        scanner.transitionTo(new ScannerStateCharContent());
    }
}

void ScannerStateFloatNumber::update(Scanner& scanner, char lexeme) {
    if (isdigit(lexeme)) {
        scanner.pushToBuffer(lexeme);

    } else if (lexeme == '.') {
        scanner.pushError("У константы Float может быть только одна точка.");
        scanner.generateToken(TokenError);
        scanner.transitionTo(new ScannerStateStart());

    } else if (isspace(lexeme)) {
        scanner.generateToken(TokenConstant);
        scanner.transitionTo(new ScannerStateStart());

    } else if (lexeme == '-' || lexeme == '+' || lexeme == '*' || lexeme == '=') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(new ScannerStateOperation());

    } else if (lexeme == '/') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(new ScannerStateCommentOrOperation());

    } else if (lexeme == ';' || lexeme == ',') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenSeparator);
        scanner.transitionTo(new ScannerStateStart());

    } else if (lexeme == '{' || lexeme == '}' || lexeme == '(' || lexeme == ')') {
        scanner.generateToken(TokenWord);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenBracket);
        scanner.transitionTo(new ScannerStateStart());

    } else if (isalpha(lexeme)) {
        scanner.pushError("Числовая константа не может содержать букв.");
        scanner.generateToken(TokenConstant);
        scanner.transitionTo(new ScannerStateStart());

    } else if (lexeme == '\'') {
        scanner.charContent = true;
        scanner.transitionTo(new ScannerStateCharContent());
    }
}

void ScannerStateOperation::update(Scanner& scanner, char lexeme) {
    if (lexeme == '+' || lexeme == '-' || lexeme == '*' || lexeme == '/' || lexeme == '=') {
        scanner.pushToBuffer(lexeme);

    } else if (isspace(lexeme)) {
        scanner.generateToken(TokenOperation);
        scanner.transitionTo(new ScannerStateStart());

    } else if (isdigit(lexeme)) {
        scanner.generateToken(TokenOperation);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(new ScannerStateNumber());

    } else if (isalpha(lexeme)) {
        scanner.generateToken(TokenOperation);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(new ScannerStateWord());

    } else if (lexeme == ';' || lexeme == ',') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenSeparator);
        scanner.transitionTo(new ScannerStateStart());

    } else if (lexeme == '{' || lexeme == '}' || lexeme == '(' || lexeme == ')') {
        scanner.generateToken(TokenWord);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenBracket);
        scanner.transitionTo(new ScannerStateStart());

    } else if (lexeme == '\'') {
        scanner.charContent = true;
        scanner.transitionTo(new ScannerStateCharContent());
    }
}

void ScannerStateCommentOrOperation::update(Scanner& scanner, char lexeme) {
    if (lexeme == '+' || lexeme == '-' || lexeme == '=') {
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(new ScannerStateOperation());

    } else if (isspace(lexeme)) {
        scanner.generateToken(TokenOperation);
        scanner.transitionTo(new ScannerStateStart());

    } else if (isdigit(lexeme)) {
        scanner.generateToken(TokenOperation);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(new ScannerStateNumber());

    } else if (isalpha(lexeme)) {
        scanner.generateToken(TokenOperation);
        scanner.pushToBuffer(lexeme);
        scanner.transitionTo(new ScannerStateWord());

    } else if (lexeme == '/') {
        scanner.clearBuffer();
        scanner.transitionTo(new ScannerStateSinglelineComment());

    } else if (lexeme == '*') {
        scanner.multilineComment = true;
        scanner.clearBuffer();
        scanner.transitionTo(new ScannerStateMultilineComment());

    } else if (lexeme == ';' || lexeme == ',') {
        scanner.generateToken(TokenConstant);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenSeparator);
        scanner.transitionTo(new ScannerStateStart());

    } else if (lexeme == '{' || lexeme == '}' || lexeme == '(' || lexeme == ')') {
        scanner.generateToken(TokenWord);
        scanner.pushToBuffer(lexeme);
        scanner.generateToken(TokenBracket);
        scanner.transitionTo(new ScannerStateStart());

    } else if (lexeme == '\'') {
        scanner.charContent = true;
        scanner.transitionTo(new ScannerStateCharContent());
    }
}

void ScannerStateSinglelineComment::update(Scanner& scanner, char lexeme) {
    if (lexeme == '\n') {
        scanner.transitionTo(new ScannerStateStart());
    }
}

void ScannerStateMultilineComment::update(Scanner& scanner, char lexeme) {
    if (lexeme == '*') {
        scanner.transitionTo(new ScannerStateFinishMultilineComment());
    }
}

void ScannerStateFinishMultilineComment::update(Scanner& scanner, char lexeme) {
    if (lexeme == '/') {
        scanner.multilineComment = false;
        scanner.transitionTo(new ScannerStateStart());
    }
}

void ScannerStateCharContent::update(Scanner& scanner, char lexeme) {
    if (lexeme == '\'') {
        scanner.charContent = false;
        scanner.generateToken(TokenConstantChar);
        scanner.transitionTo(new ScannerStateStart());

    } else {
        scanner.pushToBuffer(lexeme);
    }
}

void ScannerStateCharEscape::update(Scanner& scanner, char lexeme) {
    scanner.pushToBuffer(lexeme);
    scanner.transitionTo(new ScannerStateCharContent());
}