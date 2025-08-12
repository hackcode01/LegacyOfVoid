#include "../include/engine.hpp"

namespace Engine {
    int_fast32_t sum(int_fast32_t a, int_fast32_t b) {
        return a + b;
    }

    void printInfo() {
        ENGINE_LOG_INFO("Graphical Engine")
    }
}
