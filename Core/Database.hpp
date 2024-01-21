//
// Created by Norbert Olkowski on 15.01.2024.
//

#pragma once

#include <chrono>
#include "Engine.hpp"
#include "Writer.hpp"
#include "Reader.hpp"
#include "ClientInput.hpp"
#include "Logdb.hpp"
#include "../Structures/Structures.hpp"

class Database {
public:
    Database();

    ~Database();

    void start();

    void stop();

    void update();

    [[nodiscard]] bool runs() const;

private:
    void loadConfig();

    void readData();

    void writeData();

    Engine engine;
    Writer writer;
    Reader reader;
    ClientInput clientInput;
    Logdb logdb;
    QueryInterpreter queryInterpreter;
    Data data;
    int lastSaveCommand;
    bool is_work;
    bool is_start_read;
    bool is_start_write;
    bool is_start_force_write;
    bool is_read;
    bool is_write;
    std::chrono::steady_clock::time_point last_write_time;
    std::chrono::seconds write_interval;
};