#include "../../engine/include/engine.hpp"
#include "../../engine/include/logger.hpp"

#include <iostream>

int main() {
    ENGINE_LOG_INFO("Program main")

    std::cout << "Sum = " << Engine::sum(100, 200) << '\n';

    return 0;
}
