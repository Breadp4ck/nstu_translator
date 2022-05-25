#include <vector>
#include <string>

struct ScannerError {
    std::string msg;
    int line;
};

struct Token { };

enum TokenType {
    TokenOperation,
    TokenSeparator,
    TokenWord,
    TokenConstant,
    TokenConstantChar,
    TokenBracket,
    TokenError,
};

class Scanner;

class ScannerState {
public:
    virtual ~ScannerState() { }
    virtual void update(Scanner& scanner, char lexeme) { }
};

class Scanner {
public:
    Scanner() { };
    void scan(std::string filename);
    void transitionTo(ScannerState *newState);
    void pushToBuffer(char lexeme);
    void pushError(std::string errorMsg);
    int getLine();
    void clearBuffer();
    void generateToken(TokenType type);
    std::string getBufferAsString();
    std::vector<ScannerError> getErrors();

    bool multilineComment;
    bool charContent;

private:
    int currentLine = 1;
    ScannerState *state_;
    std::vector<char> buf;
    std::vector<Token> tokens;
    std::vector<ScannerError> errors;
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