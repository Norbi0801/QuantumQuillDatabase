//
// Created by Norbert Olkowski on 15.01.2024.
//

#include "Database.hpp"
#include <chrono>
#include "fmt/core.h"
#include "../Structures/Functions.hpp"
#include "../Parsers/LogicalExpressions.hpp"

Database::Database() : reader(&is_read), writer(&is_write), logdb("log.qql"), queryInterpreter(data),
                       engine(&logdb, &queryInterpreter), clientInput(&engine),
                       is_read(false), is_write(false), is_start_read(false), is_start_force_write(false),
                       is_start_write(false),
                       write_interval(5), lastSaveCommand(0) {
    last_write_time = std::chrono::steady_clock::now();
}

Database::~Database() {

}

void Database::start() {
    is_work = true;
    is_start_read = true;

    engine.start();
    clientInput.startInteractiveSession();
}

void Database::update() {
    auto current_time = std::chrono::steady_clock::now();
    if (is_start_read) {
        readData();
    }
    if (is_start_write || is_start_force_write ||
        std::chrono::duration_cast<std::chrono::seconds>(current_time - last_write_time) >= write_interval) {
        writeData();
        last_write_time = current_time;
    }
}

bool Database::runs() const {
    return is_work;
}

void Database::stop() {
    is_work = false;
    writeData();
}

void Database::readData() {
    is_start_read = false;
    reader.readData("database.qqb", data, lastSaveCommand);
    int test = 2;
}

void Database::writeData() {
    is_start_write = false;
    is_start_force_write = false;
    writer.writeData("database.qqb", data, logdb.getSize());
}
