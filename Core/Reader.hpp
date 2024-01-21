//
// Created by Norbert Olkowski on 15.01.2024.
//

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include "../Structures/Structures.hpp"

class Reader {
public:
    Reader(bool *readFlag) : is_read_ptr(readFlag) {}

    ~Reader() {
        if (read_thread.joinable()) {
            read_thread.join();
        }
    }

    void readData(const std::string &filePath, Data &data, int &lastCommandNumber);

private:
    void readFile(const std::string &filePath, Data *data, int *lastCommandNumber);

    std::thread read_thread;
    std::mutex mtx;
    bool *is_read_ptr;
};
