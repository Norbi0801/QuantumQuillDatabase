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

class Writer {
public:
    Writer(bool *writeFlag) : is_write_ptr(writeFlag) {}

    ~Writer() {
        if (write_thread.joinable()) {
            write_thread.join();
        }
    }

    void writeData(const std::string &filePath, const Data &data, const int &k);

private:
    void writeFile(const std::string &filePath, Data data, const int &k);

    std::thread write_thread;
    std::mutex mtx;
    bool *is_write_ptr;
};