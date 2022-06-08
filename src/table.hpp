#pragma once

#include <string>
#include <exception>
#include <list>
#include <utility>
#include <stack>
#include <vector>

#include "types.hpp"
#include "variable.hpp"

class Table { };

class ValueDoesNotExistException : public std::exception {
    public:
        const char* what() const throw() {
            return "Нет такого значения.";
        }
};

class KeyDoesNotExistException : public std::exception {
    public:
        const char* what() const throw() {
            return "Нет такого ключа.";
        }
};

template<typename TValue>
class StaticTable : Table {
    private:
        TValue *values;
        size_t size;

    public:
        StaticTable(TValue *values, size_t size);
        bool Contains(TValue value);
        size_t GetID(TValue value);
        TValue GetValue(size_t idx);
        size_t Length();
};


template<typename TKey, typename TValue>
class DynamicTable : Table {
    private:
        std::list<std::pair<TKey, TValue>> *pairs;
        size_t capacity;
        size_t size;

    public:
        DynamicTable();
        bool Contains(TKey key);
        void Update(TKey key, TValue value);
        void Remove(TKey key);
        TValue Get(TKey key);
        TValue ByHash(size_t hash);
        size_t Length();
        size_t getHash(TKey key);
};

enum TableType {
    TABLE_TYPES,
    TABLE_OPERATIONS,
    TABLE_SEPARATORS,
    TABLE_BRACKETS,
    TABLE_SPECIALS,
    TABLE_VARIABLES,
    TABLE_CONSTANTS,
    TABLES_COUNT,
};

struct Tables {
    StaticTable<std::string> *types;
    StaticTable<std::string> *operations;
    StaticTable<std::string> *separators;
    StaticTable<std::string> *brackets;
    StaticTable<std::string> *specials;
    DynamicTable<std::string, Variable> *variables;
    DynamicTable<std::string, Type> *constants;
};