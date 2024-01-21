//
// Created by Norbert Olkowski on 15.01.2024.
//

#include "Writer.hpp"
#include "../Structures/Structures.hpp"
#include "../Structures/Functions.hpp"

void Writer::writeData(const std::string &filePath, const Data &data, const int &k) {
    if (write_thread.joinable()) {
        write_thread.join();
    }
    write_thread = std::thread(&Writer::writeFile, this, filePath, data, k);
}

void Writer::writeFile(const std::string &filePath, Data data, const int &k) {
    std::lock_guard<std::mutex> lock(mtx);
    *is_write_ptr = true;
    std::ofstream file(filePath);

    try {
        saveDatabaseToFile(data, filePath, k);
    } catch (...) {
        std::cerr << "Unable to open file: " << filePath << std::endl;
    }
    *is_write_ptr = false;
}