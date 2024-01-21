//
// Created by Norbert Olkowski on 20.01.2024.
//

#include <stdexcept>
#include <iostream>
#include "Command.hpp"
#include "../Structures/Functions.hpp"
#include "fmt/core.h"

class LoadDatabaseCommand : public Command {
public:
    void execute(Data &data, const std::vector<std::string> &params, const int &lastCommandNumber) override {
        if (params.size() != 1) {
            throw std::runtime_error("Usage: load-database <file_path>");
        }
        const std::string &filePath = params[0];
        int commandNumber = 0;
        try {
            loadDatabaseFromFile(filePath, &data, &commandNumber);
            std::cout << "Database loaded from " << filePath << "\n";
        } catch (const std::exception &e) {
            fmt::print(stderr, "Failed to save database to '{}': {}\n", filePath, e.what());
        }
    }
};