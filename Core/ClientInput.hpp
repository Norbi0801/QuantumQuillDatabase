//
// Created by Norbert Olkowski on 15.01.2024.
//

#pragma once

#include <string>
#include <iostream>
#include <thread>
#include "Engine.hpp"

class ClientInput {
public:
    explicit ClientInput(Engine *engine);

    ~ClientInput();

    void startInteractiveSession();

private:
    void readConsoleInput();

    Engine *engine;
    bool session_active;
    std::thread input_thread;
};