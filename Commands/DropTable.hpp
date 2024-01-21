//
// Created by Norbert Olkowski on 19.01.2024.
//

#include "Command.hpp"
#include "fmt/core.h"
#include "../Structures/Functions.hpp"

class DropTableCommand : public Command {
public:
    void execute(Data &data, const std::vector<std::string> &params, const int &lastCommandNumber) override {
        if (params.size() != 1) {
            fmt::print(stderr, "Usage: drop-table <table_name>\n");
            return;
        }
        const std::string &tableName = params[0];
        if (dropTable(data, tableName)) {
            fmt::print("Table '{}' dropped successfully.\n", tableName);
        } else {
            fmt::print(stderr, "Failed to drop table '{}'.\n", tableName);
        }
    }
};