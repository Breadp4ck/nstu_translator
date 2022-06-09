#pragma once

#include <string>


enum TypesID {
    TYPE_INT,
    TYPE_CHAR,
    TYPE_FLOAT,
    TYPE_NULL
};


class Type {
protected:
    std::string name;
    TypesID typeID;

public:
    std::string getStr() { return name; }
    size_t type() { return typeID; }
        auto getValue() { return 0; }
};

class Int : public Type {
    int value;
    bool is_null;

    public:
        Int();
        Int(int value);
        std::string getStr();

        auto getValue() { return value; }

        static bool TryParse(std::string str, int *variable) {
            // TODO Работает только с обычными десятичными числами
            for (auto item : str) {
                if (item < '0' || item > '9') {
                    return false;
                }
            }

            *variable = std::stoi(str);
            return true;
        }
};

class Char : public Type {
    char value;
    bool is_null;

    public:
        Char();
        Char(char value);
        std::string getStr();

        auto getValue() { return value; }

        static bool TryParse(std::string str, char *variable) {
            switch (str.length()) {
                case 1:
                    if (str[0] !='\\') {
                        *variable = str[0];
                        return true;
                    }

                case 2:
                    if (str == "\\0") {
                        *variable = 0;
                        return true;

                    } else if (str == "\\t") {
                        *variable = 9;
                        return true;

                    } else if (str == "\\n") {
                        *variable = 10;
                        return true;

                    } else if (str == "\\'") {
                        *variable = '\'';
                        return true;

                    } else if (str == "\\\\") {
                        *variable = '\\';
                        return true;
                    }

                default:
                    return false;
            }

            return false;
        }
};

class Float : public Type {
    float value;
    bool is_null;

    public:
        Float();
        Float(float value);
        std::string getStr();

        auto getValue() { return value; }

        static bool TryParse(std::string str, float *variable) {
            // TODO Работает только с представлением с точкой
            for (auto item : str) {
                if ((item < '0' || item > '9') && item != '.') {
                    return false;
                }
            }

            *variable = std::stof(str);
            return true;
        }
};

class Null : public Type {
    public:
        Null() { this->typeID = TYPE_NULL; }
        std::string getStr() { return "Null"; }
};