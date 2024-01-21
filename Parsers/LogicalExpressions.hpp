//
// Created by Norbert Olkowski on 19.01.2024.
//

#pragma once

#include <vector>
#include <string>
#include <stack>
#include <stdexcept>
#include <map>
#include <ranges>

enum LogicalOperator {
    AND,
    OR
};

struct Condition {
    LogicalOperator logicalOperator;
    std::string column;
    std::string op;
    std::string value;
    bool negate = false;
};


struct Token {
    std::string value;
    bool isOperator;
};

inline std::vector<Token> toTokenVector(const std::vector<std::string> &strings) {
    std::vector<Token> tokens;
    for (const auto &str: strings) {
        tokens.push_back(Token(str, (str == "!" || str == "&&" || str == "||")));
    }
    return tokens;
}

inline std::vector<Condition> parseConditions(const std::vector<Token> &tokens) {
    std::vector<Condition> conditions;
    Condition currentCondition;
    bool isFirstCondition = true;

    for (const auto &token: tokens) {
        if (token.isOperator) {
            if (token.value == "!" && currentCondition.column.empty()) {
                currentCondition.negate = !currentCondition.negate;
                continue;
            }

            if (!currentCondition.column.empty()) {
                conditions.push_back(currentCondition);
                currentCondition = Condition();
            }

            if (token.value == "&&") {
                currentCondition.logicalOperator = LogicalOperator::AND;
            } else if (token.value == "||") {
                currentCondition.logicalOperator = LogicalOperator::OR;
            }

            isFirstCondition = false;
        } else {
            if (currentCondition.column.empty()) {
                currentCondition.column = token.value;
            } else if (currentCondition.op.empty()) {
                currentCondition.op = token.value;
            } else {
                currentCondition.value = token.value;
            }
        }
    }

    if (!currentCondition.column.empty()) {
        conditions.push_back(currentCondition);
    }

    if (!conditions.empty() && isFirstCondition) {
        conditions.front().logicalOperator = LogicalOperator::AND;
    }

    return conditions;
}


inline bool evaluateConditions(const std::vector<Condition> &conditions, Record &row) {
    bool currentResult = false;

    for (size_t i = 0; i < conditions.size(); ++i) {
        const auto &condition = conditions[i];
        auto column = condition.column;
        auto it = row.find(condition.column);
        if (it == row.end()) {
            throw std::runtime_error("Column not found in the row");
        }

        bool conditionResult;
        auto value = convertStringToDataType(condition.value);

        if (condition.op == "=") {
            conditionResult = (row[column] == value);
        } else if (condition.op == "!=") {
            conditionResult = (row[column] != value);
        } else {
            throw std::runtime_error("Unsupported comparison operator");
        }

        if (condition.negate) {
            conditionResult = !conditionResult;
        }

        if (i == 0) {
            currentResult = conditionResult;
        } else {
            if (condition.logicalOperator == LogicalOperator::AND) {
                currentResult = currentResult && conditionResult;
            } else if (condition.logicalOperator == LogicalOperator::OR) {
                currentResult = currentResult || conditionResult;
            }
        }
    }

    return currentResult;
}
