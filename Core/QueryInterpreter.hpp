//
// Created by Norbert Olkowski on 15.01.2024.
//

#pragma once

#include "fmt/core.h"
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <memory>

#include "../Structures/Structures.hpp"
#include "../Structures/Functions.hpp"
#include "../Commands/CreateTable.hpp"
#include "../Commands/DropTable.hpp"
#include "../Commands/AddColumn.hpp"
#include "../Commands/Select.hpp"
#include "../Commands/CreateRelation.hpp"
#include "../Commands/DropRelation.hpp"
#include "../Commands/AddRow.hpp"
#include "../Commands/UpdateRow.hpp"
#include "../Commands/RemoveRow.hpp"
#include "../Commands/Help.hpp"
#include "../Commands/SaveDatabase.hpp"
#include "../Commands/LoadDatabase.hpp"

class QueryInterpreter {
public:
    QueryInterpreter(Data &data) : data(data) {
        commands["create-table"] = std::make_unique<CreateTableCommand>();               // Usage: create-table <table_name>
        commands["drop-table"] = std::make_unique<DropTableCommand>();                   // Usage: drop-table <table_name>
        commands["create-relation"] = std::make_unique<CreateRelationCommand>();         // Usage: create-relation <relation_name> <table_name_1> <key_name_1> <table_name_2> <key_name_2>
        commands["drop-relation"] = std::make_unique<DropRelationCommand>();             // Usage: drop-relation <relation_name>
        commands["add-column"] = std::make_unique<AddColumnCommand>();                   // Usage: add-column <table_name> <column_name> <data_type>
        commands["add-row"] = std::make_unique<AddRowCommand>();                         // Usage: add-row <table_name> <column1_name> = <value1> <column2_name> = <value2> ...
        commands["update-row"] = std::make_unique<UpdateRowCommand>();                   // Usage: update-row <table_name> <condition1> <condition2> ... set <column1_name> = <value1> <column2_name> = <value2> ...
        commands["remove-row"] = std::make_unique<RemoveRowCommand>();                   // Usage: remove-row <table_name> <condition1> <condition2> ...
        commands["select"] = std::make_unique<SelectCommand>();                          // Usage: select <column1> <column2> ... from <table1> as <alias1> [where <condition> ...] |or| select <alias1>.<column1> <alias2>.<column2> ... from <table1> as <alias1> [merge <table2> as <alias2> on <condition> ...] [where <condition> ...]
        commands["help"] = std::make_unique<HelpCommand>();                              // Usage: help me
        commands["save-database"] = std::make_unique<SaveDatabaseCommand>();             // Usage: save-database <file_path>
        commands["load-database"] = std::make_unique<LoadDatabaseCommand>();             // Usage: load-database <file_path>

    }

    void interpret(const std::string &query, const int &lastCommandNumber) {
        params.clear();
        std::istringstream iss(query);
        std::string command;
        std::string temp;
        bool inQuotes = false;
        std::string quotedString;

        iss >> command;
        while (iss >> temp) {
            if (!inQuotes && temp.front() == '"') {
                inQuotes = true;
                quotedString = temp;

                if (temp.back() == '"' && temp.size() > 1) {
                    inQuotes = false;
                    params.push_back(quotedString);
                }
            } else if (inQuotes && temp.back() == '"') {
                inQuotes = false;
                quotedString += " " + temp;
                params.push_back(quotedString);
            } else if (inQuotes) {
                quotedString += " " + temp;
            } else {
                params.push_back(temp);
            }
        }

        if (inQuotes) {
            params.push_back(quotedString);
        }

        auto it = commands.find(command);
        if (it != commands.end()) {
            it->second->execute(data, params, lastCommandNumber);
        } else {
            fmt::print(stderr, "Error: Unknown command '{}'.\n", command);
        }
    }


private:
    Data &data;
    std::vector<std::string> params;
    std::unordered_map<std::string, std::unique_ptr<Command>> commands;
};
