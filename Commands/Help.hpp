//
// Created by Norbert Olkowski on 20.01.2024.
//

#include "Command.hpp"

class HelpCommand : public Command {
public:
    void execute(Data &data, const std::vector<std::string> &params, const int &lastCommandNumber) override {
        std::cout << "Available Commands and Usage:\n";
        std::cout << "create-table <table_name> - Creates a new table with the specified name.\n";
        std::cout << "drop-table <table_name> - Deletes the specified table.\n";
        std::cout
                << "create-relation <relation_name> <table_name_1> <key_name_1> <table_name_2> <key_name_2> - Creates a new relation between two tables.\n";
        std::cout << "drop-relation <relation_name> - Deletes the specified relation.\n";
        std::cout << "add-column <table_name> <column_name> <data_type> - Adds a new column to the specified table.\n";
        std::cout
                << "add-row <table_name> <column1_name> = <value1> <column2_name> = <value2> ... - Adds a new row to the specified table.\n";
        std::cout
                << "update-row <table_name> <condition1> <condition2> ... set <column1_name> = <value1> <column2_name> = <value2> ... - Updates rows in the specified table.\n";
        std::cout << "remove-row <table_name> <condition1> <condition2> ... - Removes rows from the specified table.\n";
        std::cout
                << "select <column1> <column2> ... from <table1> as <alias1> [where <condition> ...] - Selects data from a table.\n";
        std::cout << "save-database <file_path> - Saves the current state of the database to a file.\n";
        std::cout << "load-database <file_path> - Loads a database from a file.\n";
        std::cout << "help - Displays this help message.\n";

        std::cout << "\nDatabase Information:\n";
        std::cout << "The database allows the creation and manipulation of tables and relations.\n";
        std::cout
                << "Tables can have multiple columns of different data types, and relations can be established between tables.\n";
    }
};
