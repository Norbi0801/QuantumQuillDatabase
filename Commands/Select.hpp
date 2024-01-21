//
// Created by Norbert Olkowski on 19.01.2024.
//

#include "Command.hpp"
#include "fmt/core.h"
#include "../Structures/Functions.hpp"
#include "../Parsers/SelectQuery.hpp"

class SelectCommand : public Command {
public:
    void execute(Data &data, const std::vector<std::string> &params, const int &lastCommandNumber) override {
        if (params.empty()) {
            fmt::print(stderr,
                       "Usage: select <column1> <column2> ... from <table1> as <alias1> [where <condition> ...] |or| select <alias1>.<column1> <alias2>.<column2> ... from <table1> as <alias1> [merge <table2> as <alias2> on <condition> ...] [where <condition> ...]\n");
            return;
        }

        try {
            SelectQuery query = parseSelectQuery(params);

            Table resultTable;
            if (query.joinConditions.empty()) {
                resultTable = simpleSelect(data, query);
            } else {
                resultTable = complexSelectWithJoin(data, query);
            }

            if (!query.whereConditions.empty()) {
                TableData filteredData;
                for (auto &row: resultTable.data) {
                    if (evaluateConditions(query.whereConditions, row)) {
                        filteredData.push_back(row);
                    }
                }
                resultTable.data = filteredData;
            }

            std::string formattedResult = format_as(resultTable, query.selectedColumns);
            fmt::print("{}", formattedResult);
        } catch (const std::exception &e) {
            fmt::print(stderr, "Error: {}\n", e.what());
        }
    }

private:
    Table simpleSelect(const Data &data, const SelectQuery &query) {
        Table resultTable;

        std::unordered_map<std::string, std::string> tableAliases;
        for (const auto &[tableName, tableAlias]: query.fromTables) {
            tableAliases[tableAlias] = tableName;
        }

        for (const auto &colAlias: query.selectedColumns) {
            std::string alias, columnName;

            auto pos = colAlias.find('.');
            if (pos != std::string::npos) {
                alias = colAlias.substr(0, pos);
                columnName = colAlias.substr(pos + 1);
            } else {
                if (query.fromTables.empty()) {
                    throw std::runtime_error("No tables specified in the query");
                }
                alias = query.fromTables[0].second;
                columnName = colAlias;
            }

            if (tableAliases.find(alias) == tableAliases.end()) {
                throw std::runtime_error(fmt::format("Unknown table alias '{}'.", alias));
            }
            std::string tableName = tableAliases[alias];

            auto tableIt = data.tables.find(tableName);
            if (tableIt == data.tables.end()) {
                throw std::runtime_error(fmt::format("Table '{}' does not exist.", tableName));
            }
            const Table &sourceTable = tableIt->second;

            if (sourceTable.columns.find(columnName) == sourceTable.columns.end()) {
                throw std::runtime_error(
                        fmt::format("Column '{}' does not exist in table '{}'.", columnName, tableName));
            }
            resultTable.columns[columnName] = sourceTable.columns.at(columnName);
        }

        for (const auto &[tableName, tableAlias]: query.fromTables) {
            auto tableIt = data.tables.find(tableName);
            if (tableIt == data.tables.end()) {
                continue;
            }
            const Table &sourceTable = tableIt->second;

            for (const auto &row: sourceTable.data) {
                Record newRow;
                for (const auto &[columnName, columnInfo]: sourceTable.columns) {
                    if (row.find(columnName) != row.end()) {
                        newRow[columnName] = row.at(columnName);
                    } else {
                        newRow[columnName] = std::string("NULL");
                    }
                }
                resultTable.data.push_back(newRow);
            }
        }

        return resultTable;
    }

    Table complexSelectWithJoin(const Data &data, const SelectQuery &query) {
        Table resultTable;

        std::unordered_map<std::string, const Table *> aliasToTable;
        for (const auto &[tableName, alias]: query.fromTables) {
            aliasToTable[alias] = &data.tables.at(tableName);
        }

        std::string baseAlias = query.fromTables.front().second;
        resultTable = *aliasToTable.at(baseAlias);

        for (const auto &columnAlias: query.selectedColumns) {
            auto [alias, columnName] = splitColumnAlias(columnAlias);
            if (aliasToTable.find(alias) == aliasToTable.end()) {
                throw std::runtime_error("Alias not found: " + alias);
            }
            const auto &table = *aliasToTable.at(alias);
            const auto &columnType = table.columns.at(columnName).second;
            size_t newColumnIndex = resultTable.columns.size();
            resultTable.columns[columnName] = {newColumnIndex, columnType};
        }

        for (const auto &[left, op, right]: query.joinConditions) {
            auto [leftAlias, leftColumn] = splitColumnAlias(left);
            auto [rightAlias, rightColumn] = splitColumnAlias(right);

            if (aliasToTable.find(leftAlias) == aliasToTable.end() ||
                aliasToTable.find(rightAlias) == aliasToTable.end()) {
                throw std::runtime_error("One of the aliases in JOIN condition not found.");
            }

            const Table &leftTable = *aliasToTable.at(leftAlias);
            const Table &rightTable = *aliasToTable.at(rightAlias);

            TableData newTableData;
            for (const auto &leftRow: leftTable.data) {
                for (const auto &rightRow: rightTable.data) {
                    if (evaluateJoinCondition(leftRow, rightRow, leftColumn, rightColumn, op)) {
                        Record newRow = joinRows(leftRow, rightRow, resultTable.columns);
                        newTableData.push_back(newRow);
                    }
                }
            }
            resultTable.data = newTableData;
        }

        return resultTable;
    }

    std::pair<std::string, std::string> splitColumnAlias(const std::string &columnAlias) {
        size_t dotPosition = columnAlias.find('.');
        if (dotPosition == std::string::npos) {
            throw std::runtime_error("Invalid column alias format: " + columnAlias);
        }
        return {columnAlias.substr(0, dotPosition), columnAlias.substr(dotPosition + 1)};
    }

    static bool evaluateJoinCondition(const Record &leftRow, const Record &rightRow,
                                      const std::string &leftColumn, const std::string &rightColumn,
                                      const std::string &op) {
        auto leftValue = &leftRow.at(leftColumn);
        auto rightValue = &rightRow.at(rightColumn);

        if (!leftValue || !rightValue) {
            throw std::runtime_error("Invalid column for JOIN condition.");
        }

        if (op == "=") {
            return *leftValue == *rightValue;
        } else if (op == "!=") {
            return *leftValue != *rightValue;
        } else {
            throw std::runtime_error("Unsupported JOIN operator: " + op);
        }
    }


    Record joinRows(const Record &leftRow, const Record &rightRow,
                    const std::unordered_map<std::string, std::pair<size_t, ColumnDataType>> &resultColumns) {
        Record newRow;
        for (const auto &[columnName, columnDetails]: resultColumns) {
            if (leftRow.find(columnName) != leftRow.end()) {
                newRow[columnName] = leftRow.at(columnName);
            } else if (rightRow.find(columnName) != rightRow.end()) {
                newRow[columnName] = rightRow.at(columnName);
            } else {
                newRow[columnName] = DataType{};
            }
        }
        return newRow;
    }


};
