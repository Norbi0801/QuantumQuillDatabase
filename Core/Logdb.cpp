//
// Created by Norbert Olkowski on 16.01.2024.
//

#include <iostream>
#include "Logdb.hpp"

Logdb::Logdb(const std::string &filePath) : logFilePath(filePath), logFile(filePath, std::ios::app),
                                            logSize(countLines()) {

}

Logdb::~Logdb() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logdb::logQuery(const std::string &query) {
    std::lock_guard<std::mutex> guard(logMutex);
    if (logFile.is_open()) {
        logFile << query << '\n';
        logSize++;
        logFile.flush();
    }
}

int Logdb::countLines() {
    std::lock_guard<std::mutex> guard(logMutex);
    std::ifstream file(logFilePath);
    int lineCount = 0;
    std::string line;

    while (std::getline(file, line)) {
        ++lineCount;
    }

    return lineCount;
}

int Logdb::getSize() const {
    return logSize;
}
