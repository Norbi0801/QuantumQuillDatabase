//
// Created by Norbert Olkowski on 16.01.2024.
//

#include "Structures.hpp"
#include "Functions.hpp"
#include <algorithm>
#include <ranges>
#include <vector>
#include <sstream>
#include <fstream>
#include "fmt/format.h"

void
addColumn(Data &data, const std::string &tableName, const std::string &columnName, const std::string &dataTypeStr) {

    Table &table = data.tables[tableName];

    if (table.columns.find(columnName) != table.columns.end()) {
        throw std::invalid_argument("Column already exists");
    }

    auto it = dataTypeMap.find(dataTypeStr);
    if (it == dataTypeMap.end()) {
        throw std::invalid_argument("Unsupported column data type");
    }

    ColumnDataType columnType = it->second.first;
    DataType defaultValue = it->second.second();

    size_t newColumnIndex = table.columns.size();
    table.columns[columnName] = {newColumnIndex, columnType};

    for (auto &record: table.data) {
        record[columnName] = (defaultValue);
    }
}


void removeColumn(Table &table, const std::string &columnName) {
    auto columnIt = table.columns.find(columnName);
    if (columnIt == table.columns.end()) {
        throw std::invalid_argument("Column not found");
    }

    size_t columnIndex = columnIt->second.first;

    table.columns.erase(columnIt);

    for (auto &column: table.columns) {
        if (column.second.first > columnIndex) {
            --column.second.first;
        }
    }

    for (auto &record: table.data) {
        record.erase(columnName);
    }
}

void addRow(Table &table, const std::unordered_map<std::string, DataType> &rowData) {
    Record newRecord;

    for (const auto &[columnName, value]: rowData) {
        auto colIt = table.columns.find(columnName);
        if (colIt == table.columns.end()) {
            throw std::invalid_argument("Column '" + columnName + "' not found.\n");
            return;
        }

        const auto &columnType = colIt->second.second;

        auto dataTypeIt = std::find_if(dataTypeMap.begin(), dataTypeMap.end(),
                                       [columnType](const auto &pair) { return pair.second.first == columnType; });

        if (dataTypeIt == dataTypeMap.end()) {
            throw std::invalid_argument("Unknown data type for column '" + columnName + "'.\n");
        }

        bool typeMatch = (columnType == ColumnDataType::Integer && std::holds_alternative<int>(value)) ||
                         (columnType == ColumnDataType::String && std::holds_alternative<std::string>(value));

        if (!typeMatch) {
            throw std::invalid_argument("Type mismatch in column '" + columnName + "'.\n");
        }

        newRecord[columnName] = value;
    }

    table.data.push_back(newRecord);
}


void removeRow(Table &table, size_t rowIndex) {
    if (rowIndex < table.data.size()) {
        table.data.erase(table.data.begin() + rowIndex);
    } else {
        throw std::invalid_argument("Row index out of range.\n");
    }
}

void removeRowsIf(Table &table, const std::vector<Condition> &conditions) {
    auto it = std::remove_if(table.data.begin(), table.data.end(),
                             [&](Record &record) {
                                 return evaluateConditions(conditions, record);
                             });
    table.data.erase(it, table.data.end());
}

std::string format_as(const Table &table, const std::vector<std::string> &columnOrder) {
    std::string formattedTable;

    for (const auto &colAlias: columnOrder) {
        auto pos = colAlias.find('.');
        std::string columnName = pos != std::string::npos ? colAlias.substr(pos + 1) : colAlias;
        auto colIt = table.columns.find(columnName);
        if (colIt != table.columns.end()) {
            formattedTable += fmt::format("{} ({}), ", colIt->first, convertColumnTypeToString(colIt->second.second));
        }
    }
    formattedTable += "\n";
    formattedTable += std::string(columnOrder.size() * 22, '-') + "\n";

    for (const auto &row: table.data) {
        for (const auto &colAlias: columnOrder) {
            auto pos = colAlias.find('.');
            std::string columnName = pos != std::string::npos ? colAlias.substr(pos + 1) : colAlias;
            auto dataIt = row.find(columnName);
            if (dataIt != row.end()) {
                std::visit([&formattedTable](const auto &value) {
                    formattedTable += fmt::format("{}, ", value);
                }, dataIt->second);
            } else {
                formattedTable += "NULL, ";
            }
        }
        formattedTable += "\n";
    }

    return formattedTable;
}


void createTable(Data &data, const std::string &tableName) {
    if (data.tables.find(tableName) != data.tables.end()) {
        throw std::invalid_argument(fmt::format("Table '{}' already exists.", tableName));
    }
    Table newTable;
    data.tables[tableName] = newTable;
}

void
createRelation(Data &data, const std::string &relationName, const std::string &tableName1, const std::string &keyName1,
               const std::string &tableName2, const std::string &keyName2) {
    if (data.relations.find(relationName) != data.relations.end()) {
        throw std::invalid_argument(fmt::format("Relation '{}' already exists.", relationName));
    }

    auto validate = [&data](const std::string &table, const std::string &key) {
        if (data.tables[table].columns.find(key) != data.tables[table].columns.end()) {
            throw std::invalid_argument(fmt::format("Column '{}' not exist in {}", table, key));
        }
    };

    validate(tableName1, keyName1);
    validate(tableName2, keyName2);

    data.relations[relationName] = {
            RelationAtom(tableName1, keyName1),
            RelationAtom(tableName2, keyName2)
    };
}

bool dropTable(Data &data, const std::string &tableName) {
    auto it = data.tables.find(tableName);

    if (std::ranges::any_of(data.relations, [tableName](const auto &pair) {
        const Relation &elem = pair.second;
        return elem.first.table == tableName || elem.second.table == tableName;
    })) {
        throw std::invalid_argument("Table '" + tableName + "' has relation.\n");
    }

    if (it == data.tables.end()) {
        throw std::invalid_argument("Table '" + tableName + "' does not exist.\n");
    }

    data.tables.erase(it);
    return true;
}

void updateTableRows(Table &table, const std::vector<Condition> &conditions,
                     const std::unordered_map<std::string, DataType> &updates) {
    for (auto &row: table.data) {
        if (evaluateConditions(conditions, row)) {
            for (const auto &[columnName, newValue]: updates) {
                row[columnName] = newValue;
            }
        }
    }
}

void loadDatabaseFromFile(const std::string &filePath, Data *data, int *lastCommandNumber) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        throw std::runtime_error("Unable to open file: " + filePath);
    }

    std::string line;
    if (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string temp;
        std::string readNumber;
        if (!(iss >> temp >> temp >> temp >> readNumber)) {
            throw std::runtime_error("Failed to read last command number");
        }
        if (lastCommandNumber != nullptr) {
            *lastCommandNumber = std::stoi(readNumber);
        }
    }

    std::string currentTableName;
    Table currentTable;

    while (std::getline(inFile, line)) {
        if (line.rfind("Table: ", 0) == 0) {
            if (!currentTableName.empty()) {
                data->tables[currentTableName] = currentTable;
                currentTable = Table();
            }
            currentTableName = line.substr(7);
        } else if (!line.empty() && line.find('(') != std::string::npos) {
            std::istringstream columnStream(line);
            std::string columnInfo;
            while (std::getline(columnStream, columnInfo, ',')) {
                size_t pos = columnInfo.find('(');
                std::string columnName = columnInfo.substr(0, pos);
                std::string columnTypeStr = columnInfo.substr(pos + 1, columnInfo.find(')') - pos - 1);

                ColumnDataType columnType;
                if (columnTypeStr == "int") {
                    columnType = ColumnDataType::Integer;
                } else if (columnTypeStr == "string") {
                    columnType = ColumnDataType::String;
                } else {
                    throw std::runtime_error("Unknown column type: " + columnTypeStr);
                }

                currentTable.columns[columnName] = {currentTable.columns.size(), columnType};
            }
        } else if (!line.empty()) {
            std::istringstream recordStream(line);
            std::string value;
            Record record;
            size_t columnIndex = 0;
            while (std::getline(recordStream, value, ',')) {
                auto columnTypeIt = std::find_if(currentTable.columns.begin(), currentTable.columns.end(),
                                                 [columnIndex](const auto &pair) {
                                                     return pair.second.first == columnIndex;
                                                 });
                if (columnTypeIt == currentTable.columns.end()) {
                    throw std::runtime_error("Invalid column index in record");
                }

                if (columnTypeIt->second.second == ColumnDataType::Integer) {
                    try {
                        record[columnTypeIt->first] = std::stoi(value);
                    } catch (...) {
                        record[columnTypeIt->first] = value;
                    }
                } else {
                    record[columnTypeIt->first] = value;
                }

                columnIndex++;
            }
            currentTable.data.push_back(record);
        }
    }

    if (!currentTableName.empty()) {
        data->tables[currentTableName] = currentTable;
    }
}

void saveDatabaseToFile(Data data, const std::string &filePath, const int lastCommandNumber) {
    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        throw std::runtime_error("Unable to open file for writing: " + filePath);
    }

    outFile << "Last Command Number: " << lastCommandNumber << "\n";

    for (const auto &[tableName, table]: data.tables) {
        outFile << "Table: " << tableName << "\n";

        bool firstColumn = true;
        for (const auto &[columnName, columnInfo]: table.columns) {
            if (!firstColumn) {
                outFile << ",";
            }
            outFile << columnName << "(" << convertColumnTypeToString(columnInfo.second) << ")";
            firstColumn = false;
        }
        outFile << "\n";

        for (const auto &record: table.data) {
            bool firstValue = true;
            for (const auto &[columnName, columnInfo]: table.columns) {
                if (!firstValue) {
                    outFile << ",";
                }
                auto valueIt = record.find(columnName);
                if (valueIt != record.end()) {
                    std::visit([&outFile](const auto &value) {
                        outFile << value;
                    }, valueIt->second);
                } else {
                    outFile << "NULL";
                }
                firstValue = false;
            }
            outFile << "\n";
        }
        outFile << "\n";
    }
}
