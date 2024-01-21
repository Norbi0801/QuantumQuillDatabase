//
// Created by Norbert Olkowski on 16.01.2024.
//

#pragma once

#include "Structures.hpp"
#include "../Parsers/LogicalExpressions.hpp"

void createTable(Data &data, const std::string &tableName);

bool dropTable(Data &data, const std::string &tableName);

void
createRelation(Data &data, const std::string &relationName, const std::string &tableName1, const std::string &keyName1,
               const std::string &tableName2, const std::string &keyName2);

void addColumn(Data &data, const std::string &tableName, const std::string &columnName, const std::string &dataTypeStr);

void removeColumn(Table &table, const std::string &columnName);

void addRow(Table &table, const std::unordered_map<std::string, DataType> &rowData);

void removeRow(Table &table, size_t rowIndex);

void removeRowsIf(Table &table, const std::vector<Condition> &conditions);

std::string format_as(const Table &table, const std::vector<std::string> &columnOrder);

void updateTableRows(Table &table, const std::vector<Condition> &conditions,
                     const std::unordered_map<std::string, DataType> &updates);

void loadDatabaseFromFile(const std::string &filePath, Data *data, int *lastCommandNumber);

void saveDatabaseToFile(Data data, const std::string &filePath, const int lastCommandNumber);