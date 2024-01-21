//
// Created by Norbert Olkowski on 15.01.2024.
//

#pragma once

#include <string>
#include "Logdb.hpp"
#include "QueryInterpreter.hpp"

class Engine {
public:
    Engine();

    Engine(Logdb *pLogdb, QueryInterpreter *pQueryInterpreter);

    ~Engine();

    void start();

    void processInput(const std::string &input) const;

    [[nodiscard]] bool runs() const;

private:
    bool is_work;
    Logdb *pLogdb{};
    QueryInterpreter *pQueryInterpreter{};
};