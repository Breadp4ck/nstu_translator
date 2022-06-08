#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "table.hpp"
#include "types.hpp"
#include "token.hpp"

struct ScannerError {
    std::string msg;
    int line;
};

class Scanner;

class ScannerState {
public:
    virtual ~ScannerState() { }
    virtual void update(Scanner& scanner, char lexeme) { }
};

class Scanner {
public:
    Scanner(Tables tables);
    void scan(std::string filename);
    void transitionTo(ScannerState *newState);
    void pushToBuffer(char lexeme);
    void pushError(std::string errorMsg);
    int getLine();
    void clearBuffer();
    void generateToken(TokenType type);
    std::string getBufferAsString();
    std::vector<ScannerError> getErrors();
    std::vector<Token> getTokens();
    std::vector<size_t> getTokenLineIndeces();

    bool multilineComment = false;
    bool charContent = false;

private:
    int currentLine = 1;
    int currentTokenID = 0;
    ScannerState *state_;
    Tables tables_;
    std::vector<char> buf;
    std::vector<Token> tokens;
    std::vector<ScannerError> errors;
    std::vector<size_t> tokenLineIndeces;
};

class ScannerStateStart : public ScannerState {
public:
    ScannerStateStart() : ScannerState() { }
    virtual void update(Scanner& scanner, char lexeme);
};

class ScannerStateWord : public ScannerState {
public:
    ScannerStateWord() : ScannerState() { }
    virtual void update(Scanner& scanner, char lexeme);
};

class ScannerStateNumber : public ScannerState {
public:
    ScannerStateNumber() : ScannerState() { }
    virtual void update(Scanner& scanner, char lexeme);
};

class ScannerStateFloatNumber : public ScannerState {
public:
    ScannerStateFloatNumber() : ScannerState() { }
    virtual void update(Scanner& scanner, char lexeme);
};

class ScannerStateOperation : public ScannerState {
public:
    ScannerStateOperation() : ScannerState() { }
    virtual void update(Scanner& scanner, char lexeme);
};

class ScannerStateCommentOrOperation : public ScannerState {
public:
    ScannerStateCommentOrOperation() : ScannerState() { }
    virtual void update(Scanner& scanner, char lexeme);
};

class ScannerStateSinglelineComment : public ScannerState {
public:
    ScannerStateSinglelineComment() : ScannerState() { }
    virtual void update(Scanner& scanner, char lexeme);
};

class ScannerStateMultilineComment : public ScannerState {
public:
    ScannerStateMultilineComment() : ScannerState() { }
    virtual void update(Scanner& scanner, char lexeme);
};

class ScannerStateFinishMultilineComment : public ScannerState {
public:
    ScannerStateFinishMultilineComment() : ScannerState() { }
    virtual void update(Scanner& scanner, char lexeme);
};

class ScannerStateCharContent : public ScannerState {
public:
    ScannerStateCharContent() : ScannerState() { }
    virtual void update(Scanner& scanner, char lexeme);
};

class ScannerStateCharEscape : public ScannerState {
public:
    ScannerStateCharEscape() : ScannerState() { }
    virtual void update(Scanner& scanner, char lexeme);
};