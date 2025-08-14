#include "../../engine/include/Logger.hpp"

#include "../../engine/include/core/application/Application.hpp"

int main() {
    ENGINE_LOG_INFO("Main program")

    try {
        Engine::Application app{};
        app.run();
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
