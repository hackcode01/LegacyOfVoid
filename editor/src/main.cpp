#include "../../engine/include/Logger.hpp"

#include "../../engine/include/core/application/Application.hpp"

#include <iostream>

using namespace Engine;

int main() {
    ENGINE_LOG_INFO("Main program")

    try {
        Application app{};
        app.run();
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
