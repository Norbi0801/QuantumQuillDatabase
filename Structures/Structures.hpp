
//
// Created by Norbert Olkowski on 16.01.2024.
//

#pragma once

#include <vector>
#include <variant>
#include <string>
#include <unordered_map>
#include <functional>
#include "Types.hpp"

using Record = std::unordered_map<std::string, DataType>;

using TableData = std::vector<Record>;

struct Table {
    std::unordered_map<std::string, std::pair<size_t, ColumnDataType>> columns;
    TableData data;
};

struct RelationAtom {
    std::string table;
    std::string key;
};

struct Relation {
    RelationAtom first;
    RelationAtom second;
};

struct Data {
    std::unordered_map<std::string, Table> tables;
    std::unordered_map<std::string, Relation> relations;
};
