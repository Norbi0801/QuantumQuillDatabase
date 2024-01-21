#include "Core/Database.hpp"

int main() {
    auto database = Database();
    database.start();
    while (database.runs()) {
        database.update();
    }
    return 0;
}
