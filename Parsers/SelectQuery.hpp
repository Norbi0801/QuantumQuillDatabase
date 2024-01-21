//
// Created by Norbert Olkowski on 19.01.2024.
//

#include <vector>
#include <string>
#include <sstream>
#include <tuple>

struct SelectQuery {
    std::vector<std::string> selectedColumns;
    std::vector<std::pair<std::string, std::string>> fromTables;
    std::vector<std::tuple<std::string, std::string, std::string>> joinConditions;
    std::vector<Condition> whereConditions;
};

inline SelectQuery parseSelectQuery(const std::vector<std::string> &params) {
    SelectQuery query;
    enum State {
        ParsingColumns,
        ParsingFrom,
        ParsingJoin,
        ParsingOn,
        ExpectingAlias,
        ParsingWhere
    };

    State currentState = ParsingColumns;
    std::string lastTableName;
    std::vector<std::string> whereParams;
    std::string A, B, C;
    int onConditionPart = 0;

    for (const auto &param: params) {
        if (param == "from") {
            currentState = ParsingFrom;
            continue;
        } else if (param == "merge") {
            currentState = ParsingJoin;
            continue;
        } else if (param == "on") {
            currentState = ParsingOn;
            continue;
        } else if (param == "as") {
            currentState = ExpectingAlias;
            continue;
        } else if (param == "where") {
            currentState = ParsingWhere;
            continue;
        }

        switch (currentState) {
            case ParsingColumns:
                query.selectedColumns.push_back(param);
                break;
            case ParsingFrom:
            case ParsingJoin:
                lastTableName = param;
                break;
            case ExpectingAlias:
                query.fromTables.emplace_back(lastTableName, param);
                currentState = ParsingJoin;
                break;
            case ParsingOn:
                if (onConditionPart == 0) {
                    A = param;
                    onConditionPart++;
                } else if (onConditionPart == 1) {
                    B = param;
                    onConditionPart++;
                } else if (onConditionPart == 2) {
                    C = param;
                    query.joinConditions.emplace_back(A, B, C);
                    onConditionPart = 0;
                }
                break;
            case ParsingWhere:
                whereParams.push_back(param);
                break;
        }
    }

    if (!whereParams.empty()) {
        std::vector<Token> tokens = toTokenVector(whereParams);
        query.whereConditions = parseConditions(tokens);
    }

    return query;
}

