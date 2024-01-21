//
// Created by norbe on 19.01.2024.
//

#pragma once

#include <unordered_map>
#include <variant>
#include <string>
#include <functional>

using DataType = std::variant<int, std::string>;

enum class ColumnDataType {
    Integer,
    String
};

inline std::unordered_map<std::string, std::pair<ColumnDataType, std::function<DataType()>>> dataTypeMap = {
        {"int",    {ColumnDataType::Integer, []() -> DataType { return int{}; }}},
        {"string", {ColumnDataType::String,  []() -> DataType { return std::string{}; }}}
};

inline ColumnDataType inferColumnType(const std::string &input) {
    if (input.size() >= 2 && input.front() == '"' && input.back() == '"') {
        return ColumnDataType::String;
    }

    size_t pos = 0;
    try {
        std::stoi(input, &pos);
        if (pos == input.size()) {
            return ColumnDataType::Integer;
        }
    } catch (...) {
        return ColumnDataType::String;
    }


    return ColumnDataType::String;
}

inline DataType convertStringToDataType(const std::string &input) {
    ColumnDataType type = inferColumnType(input);

    if (type == ColumnDataType::Integer) {
        if (input.find_first_not_of("0123456789") == std::string::npos) {
            try {
                return std::stoi(input);
            } catch (...) {
                return input;
            }
        } else {
            return input;
        }
    } else {
        if (input.size() >= 2 && input.front() == '"' && input.back() == '"') {
            return input.substr(1, input.size() - 2);
        } else {
            return input;
        }
    }
}

inline std::string convertColumnTypeToString(const ColumnDataType &columnType) {
    switch (columnType) {
        case ColumnDataType::Integer:
            return "int";
        case ColumnDataType::String:
            return "string";
        default:
            return "unknown";
    }
}