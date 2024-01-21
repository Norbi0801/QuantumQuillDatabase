//
// Created by Norbert Olkowski on 15.01.2024.
//
#include "Engine.hpp"
#include "Logdb.hpp"

#include <iostream>

Engine::Engine() : is_work(false) {}

Engine::Engine(Logdb *pLogdb, QueryInterpreter *pQueryInterpreter) : is_work(false), pLogdb(pLogdb),
                                                                     pQueryInterpreter(pQueryInterpreter) {}

void Engine::start() {
    is_work = true;
}

void Engine::processInput(const std::string &input) const {
    if (!is_work) {
        return;
    }
    pQueryInterpreter->interpret(input, pLogdb->getSize());
    pLogdb->logQuery(input);
}

bool Engine::runs() const {
    return is_work;
}

Engine::~Engine() {

}
