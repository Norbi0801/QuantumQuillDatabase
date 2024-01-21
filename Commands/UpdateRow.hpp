//
// Created by Norbert Olkowski on 19.01.2024.
//

#include "Command.hpp"
#include "fmt/core.h"
#include "../Structures/Functions.hpp"
#include "../Parsers/LogicalExpressions.hpp"

class UpdateRowCommand : public Command {
public:
    void execute(Data &data, const std::vector<std::string> &params, const int &lastCommandNumber) override {
        if (params.size() < 4) {
            fmt::print(stderr,
                       "Usage: update-row <table_name> <condition1> <condition2> ... set <column1_name> = <value1> <column2_name> = <value2> ...\n");
            return;
        }

        const std::string &tableName = params[0];
        auto tableIt = data.tables.find(tableName);
        if (tableIt == data.tables.end()) {
            fmt::print(stderr, "Error: Table '{}' does not exist.\n", tableName);
            return;
        }

        try {
            Table &table = tableIt->second;

            size_t setIndex = std::distance(params.begin(), std::find(params.begin(), params.end(), "set"));
            std::vector<std::string> conditionParams(params.begin() + 1, params.begin() + setIndex);
            std::vector<Token> tokens = toTokenVector(conditionParams);
            std::vector<Condition> conditions = parseConditions(tokens);

            std::unordered_map<std::string, DataType> updates;
            for (size_t i = setIndex + 1; i < params.size(); i += 3) {
                const std::string &columnName = params[i];
                const std::string &valueStr = params[i + 2];
                DataType value = convertStringToDataType(valueStr);
                updates[columnName] = value;
            }
            updateTableRows(table, conditions, updates);
            fmt::print("Row added to table '{}' successfully.\n", tableName);
        } catch (const std::exception &e) {
            fmt::print(stderr, "Error: {}\n", e.what());
        }
    }
};