//
// Created by norbe on 19.01.2024.
//

#pragma once

#include "Command.hpp"
#include "fmt/core.h"
#include "../Structures/Functions.hpp"

class CreateTableCommand : public Command {
public:
    void execute(Data &data, const std::vector<std::string> &params, const int& lastCommandNumber) override {
        if (params.size() != 1) {
            fmt::print(stderr, "Usage: create-table <table_name>\n");
            return;
        }
        const std::string &tableName = params[0];
        try {
            createTable(data, tableName);
            fmt::print("Table '{}' created successfully.\n", tableName);
        } catch (const std::exception &e) {
            fmt::print(stderr, "Error: {}\n", e.what());
        }
    }
};
