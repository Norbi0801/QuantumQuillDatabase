//
// Created by Norbert Olkowski on 19.01.2024.
//

#include "Command.hpp"
#include "fmt/core.h"
#include "../Structures/Functions.hpp"

class AddColumnCommand : public Command {
public:
    void execute(Data &data, const std::vector<std::string> &params, const int& lastCommandNumber) override {
        if (params.size() != 3) {
            fmt::print(stderr, "Usage: add-column <table_name> <column_name> <data_type>\n");
            return;
        }
        const std::string &tableName = params[0];
        const std::string &columnName = params[1];
        const std::string &dataType = params[2];
        try {
            addColumn(data, tableName, columnName, dataType);
            fmt::print("Column '{}' added to table '{}' successfully.\n", columnName, tableName);
        } catch (const std::out_of_range &) {
            fmt::print(stderr, "Error: Table '{}' does not exist.\n", tableName);
        } catch (const std::exception &e) {
            fmt::print(stderr, "Error: {}\n", e.what());
        }
    }
};