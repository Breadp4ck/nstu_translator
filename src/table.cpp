#include "table.hpp"
#include "types.hpp"
#include "variable.hpp"
#include <vector>
#include <iostream>

// -----------------------------------------------------------
// STATIC TABLE
// -----------------------------------------------------------

template<typename TValue>
StaticTable<TValue>::StaticTable(TValue *values, size_t size) {
    this->size = size;
    this->values = new TValue[size];            

    for (size_t i = 0; i < this->size; i++) {
        this->values[i] = values[i];
    }
}

template<typename TValue>
bool StaticTable<TValue>::Contains(TValue value) {
    for (size_t i = 0; i < this->size; i++) {
        if (this->values[i] == value) {
            return true;
        }
    }

    return false;
}

template<typename TValue>
size_t StaticTable<TValue>::GetID(TValue value) {
    for (size_t i = 0; i < this->size; i++) {
        if (this->values[i] == value) {
            return i;
        }
    }

    throw ValueDoesNotExistException();
}

template<typename TValue>
TValue StaticTable<TValue>::GetValue(size_t idx) {
    if (idx < this->size) {
        return this->values[idx];
    }

    throw ValueDoesNotExistException();
}

template<typename TValue>
size_t StaticTable<TValue>::Length() {
    return size;
}


// -----------------------------------------------------------
// DYNAMIC TABLE
// -----------------------------------------------------------

template<typename TKey, typename TValue>
DynamicTable<TKey, TValue>::DynamicTable() {
    this->capacity = 1024;
    this->pairs = new std::list<std::pair<TKey, TValue>>[capacity];
    this->size = 0;
}

template<typename TKey, typename TValue>
bool DynamicTable<TKey, TValue>::Contains(TKey key) {
    size_t hash = getHash(key);
    auto& cell = this->pairs[hash];

    for (auto pair = begin(cell); pair != end(cell); pair++) {
        if (pair->first == key) {
            return true;
        }
    }

    return false;
}

template<typename TKey, typename TValue>
void DynamicTable<TKey, TValue>::Update(TKey key, TValue value) {
    size_t hash = getHash(key);
    auto& cell = this->pairs[hash];

    // Key doesn't exist
    if (cell.size() == 0) {
        if (this->size == this->capacity) {
            this->capacity *= 2;
        }
        
        cell.emplace_back(key, value);
        this->size += 1;

        keys.push_back(key);

    // Key exists
    } else {
        for (auto pair = begin(cell); pair != end(cell); pair++) {
            if (pair->first == key) {
                pair->second = value;
                break;
            }
        }
    }

}

template<typename TKey, typename TValue>
void DynamicTable<TKey, TValue>::Remove(TKey key) {
    size_t hash = getHash(key);
    auto& cell = this->pairs[hash];

    if (cell.size() == 0) {
        throw KeyDoesNotExistException();
    }

    for (auto pair = begin(cell); pair != end(cell); pair++) {
        if (pair->first == key) {
            pair = cell.erase(pair);
            this->size -= 1;
            return;
        }
    }
}

template<typename TKey, typename TValue>
TValue DynamicTable<TKey, TValue>::Get(TKey key) {
    size_t hash = getHash(key);
    auto& cell = this->pairs[hash];

    if (cell.size() == 0) {
        throw KeyDoesNotExistException();
    }

    for (auto pair = begin(cell); pair != end(cell); pair++) {
        if (pair->first == key) {
            return pair->second;
        }
    }

    throw KeyDoesNotExistException();
}

template<typename TKey, typename TValue>
TValue DynamicTable<TKey, TValue>::ByHash(size_t hash) {
    auto& cell = this->pairs[hash];

    if (cell.size() == 0) {
        throw KeyDoesNotExistException();
    }

    auto pair = begin(cell);
    return pair->second;
}

template<typename TKey, typename TValue>
size_t DynamicTable<TKey, TValue>::Length() {
    return size;
}

template<typename TKey, typename TValue>
size_t DynamicTable<TKey, TValue>::getHash(TKey key) {
    return std::hash<TKey>()(key) % capacity;
}

template<typename TKey, typename TValue>
std::vector<TKey> DynamicTable<TKey, TValue>::getKeys() {
    return keys;
}

template class StaticTable<std::string>;
template class DynamicTable<std::string, Type>;
template class DynamicTable<std::string, Variable>;
template class DynamicTable<size_t, Variable>;