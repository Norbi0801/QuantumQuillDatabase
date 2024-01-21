//
// Created by norbe on 19.01.2024.
//

#pragma once

#include "Command.hpp"
#include "fmt/core.h"
#include "../Structures/Functions.hpp"

class CreateRelationCommand : public Command {
public:
    void execute(Data &data, const std::vector<std::string> &params, const int &lastCommandNumber) override {
        if (params.size() != 5) {
            fmt::print(stderr,
                       "Usage: create-relation <relation_name> <table_name_1> <key_name_1> <table_name_2> <key_name_2>\n");
            return;
        }
        try {
            createRelation(data, params[0], params[1], params[2], params[3], params[4]);
            fmt::print("Relation '{}' created successfully.\n", params[0]);
        } catch (const std::exception &e) {
            fmt::print(stderr, "Error: {}\n", e.what());
        }
    }
};
