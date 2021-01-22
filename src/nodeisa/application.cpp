#include "application.hpp"

#include <iostream>
#include <emscripten.h>
void EMSCRIPTEN_KEEPALIVE Application::Initialize() {
    std::cout << "Initializing application." << std::endl;
}

void EMSCRIPTEN_KEEPALIVE Application::SayHello() {
    std::cout << "Hello!" << std::endl;
}
