//
// Created by Norbert Olkowski on 15.01.2024.
//

#pragma once

#include <string>
#include <mutex>
#include <fstream>

class Logdb {
public:
    Logdb(const std::string &logFilePath);

    ~Logdb();

    void logQuery(const std::string &query);

    int getSize() const;

private:
    int countLines();

    std::string logFilePath;
    std::mutex logMutex;
    std::ofstream logFile;
    int logSize;
};
