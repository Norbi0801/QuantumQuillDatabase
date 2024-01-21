//
// Created by Norbert Olkowski on 15.01.2024.
//

#include "Reader.hpp"
#include "../Structures/Functions.hpp"

void Reader::readData(const std::string &filePath, Data &data, int& lastCommandNumber) {
    if (read_thread.joinable()) {
        read_thread.join();
    }
    read_thread = std::thread(&Reader::readFile, this, filePath, &data, &lastCommandNumber);
}

void Reader::readFile(const std::string &filePath, Data *data, int *lastCommandNumber) {
    std::lock_guard<std::mutex> lock(mtx);
    *is_read_ptr = true;

    try {
        loadDatabaseFromFile(filePath, data, lastCommandNumber);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    *is_read_ptr = false;
}