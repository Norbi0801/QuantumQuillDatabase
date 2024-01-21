//
// Created by Norbert Olkowski on 19.01.2024.
//

#pragma once

#include <vector>
#include "../Structures/Structures.hpp"

class Command {
public:
    virtual ~Command() = default;

    virtual void execute(Data &data, const std::vector<std::string> &params, const int& lastCommandNumber) = 0;
};