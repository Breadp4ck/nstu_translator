#include "variable.hpp"
#include "types.hpp"

Variable::Variable(std::string varName) {
    this->data = Null();
    this->typeID = TYPE_NULL;
    this->varName = varName;
}

std::string Variable::name() {
    return this->varName;
}

TypesID Variable::type() {
    return this->typeID;
}

void Variable::init(TypesID id) {
    this->typeID = id;

    switch (id) {
        case TYPE_NULL:
            this->data = Null();
            break;

        case TYPE_INT:
            this->data = Int(0);
            break;

        case TYPE_CHAR:
            this->data = Char(0);
            break;

        case TYPE_FLOAT:
            this->data = Float(0);
            break;
    }
}