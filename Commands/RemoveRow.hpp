//
// Created by norbe on 19.01.2024.
//

#include "Command.hpp"
#include "fmt/core.h"
#include "../Structures/Functions.hpp"

class RemoveRowCommand : public Command {
public:
    void execute(Data &data, const std::vector<std::string> &params, const int &lastCommandNumber) override {
        if (params.size() < 2) {
            fmt::print(stderr, "Usage: remove-row <table_name> <condition1> <condition2> ...\n");
            return;
        }

        const std::string &tableName = params[0];
        auto tableIt = data.tables.find(tableName);
        if (tableIt == data.tables.end()) {
            fmt::print(stderr, "Error: Table '{}' does not exist.\n", tableName);
            return;
        }
        Table &table = tableIt->second;

        std::vector<std::string> conditionParams(params.begin() + 1, params.end());
        std::vector<Token> tokens = toTokenVector(conditionParams);
        std::vector<Condition> conditions = parseConditions(tokens);

        try {
            removeRowsIf(table, conditions);
            fmt::print("Rows removed from table '{}' successfully.\n", tableName);
        } catch (const std::exception &e) {
            fmt::print(stderr, "Error: {}\n", e.what());
        }
    }
};

