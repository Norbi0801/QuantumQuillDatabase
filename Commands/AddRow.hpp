//
// Created by Norbert Olkowski on 19.01.2024.
//

#include "Command.hpp"
#include "fmt/core.h"
#include "../Structures/Functions.hpp"

class AddRowCommand : public Command {
public:
    void execute(Data &data, const std::vector<std::string> &params, const int& lastCommandNumber) override {
        if (params.size() < 4) {
            fmt::print(stderr, "Usage: add-row <table_name> <column1_name> = <value1> <column2_name> = <value2> ...\n");
            return;
        }
        const std::string &tableName = params[0];
        if (data.tables.find(tableName) == data.tables.end()) {
            fmt::print(stderr, "Error: Table '{}' does not exist.\n", tableName);
            return;
        }
        Table &table = data.tables[tableName];

        std::unordered_map<std::string, DataType> rowData;
        for (size_t i = 1; i < params.size(); i += 3) {
            const std::string &columnName = params[i];
            const std::string &valueStr = params[i + 2];
            DataType value = convertStringToDataType(valueStr);
            rowData[columnName] = value;
        }

        try {
            addRow(table, rowData);
            fmt::print("Row added to table '{}' successfully.\n", tableName);
        } catch (const std::exception &e) {
            fmt::print(stderr, "Error: {}\n", e.what());
        }
    }
};
