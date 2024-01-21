//
// Created by Norbert Olkowski on 15.01.2024.
//

#include "ClientInput.hpp"
#include "Engine.hpp"

ClientInput::ClientInput(Engine *eng) : engine(eng), session_active(false) {}

ClientInput::~ClientInput() {
    if (session_active) {
        session_active = false;
        if (input_thread.joinable()) {
            input_thread.join();
        }
    }
}

void ClientInput::startInteractiveSession() {
    session_active = true;
    input_thread = std::thread(&ClientInput::readConsoleInput, this);
}

void ClientInput::readConsoleInput() {
    std::string input;
    while (session_active) {
        std::cout << "\nqq>";
        std::getline(std::cin, input);
        if (!input.empty()) {
            engine->processInput(input);
        }
    }
}