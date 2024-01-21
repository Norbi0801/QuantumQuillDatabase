//
// Created by Norbert Olkowski on 19.01.2024.
//

#include "Command.hpp"
#include "fmt/core.h"
#include "../Structures/Functions.hpp"

class SaveDatabaseCommand : public Command {
public:
    void execute(Data &data, const std::vector<std::string> &params, const int &lastCommandNumber) override {
        if (params.size() != 1) {
            fmt::print(stderr, "Usage: save-database <file_path>\n");
            return;
        }
        const std::string &filePath = params[0];
        try {
            saveDatabaseToFile(data, filePath, lastCommandNumber);
            fmt::print("Database saved to '{}' successfully.\n", filePath);
        } catch (const std::exception &e) {
            fmt::print(stderr, "Failed to save database to '{}': {}\n", filePath, e.what());
        }
    }
};