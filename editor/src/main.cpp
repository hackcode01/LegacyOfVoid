#include "../../engine/include/Logger.hpp"

#include "../../engine/include/core/application/Application.hpp"

#include <iostream>

int main() {
    ENGINE_LOG_INFO("Main program")

    Engine::Application app{};
    app.run();

    return 0;
}
