//
// Created by Norbert Olkowski on 19.01.2024.
//

#include "Command.hpp"
#include "fmt/core.h"
#include "../Structures/Functions.hpp"

class DropRelationCommand : public Command {
public:
    void execute(Data &data, const std::vector<std::string> &params, const int &lastCommandNumber) override {
        if (params.size() != 1) {
            fmt::print(stderr, "Usage: drop-relation <relation_name>\n");
            return;
        }
        const std::string &relationName = params[0];
        if (dropTable(data, relationName)) {
            fmt::print("Relation '{}' dropped successfully.\n", relationName);
        } else {
            fmt::print(stderr, "Failed to drop relation '{}'.\n", relationName);
        }
    }
};