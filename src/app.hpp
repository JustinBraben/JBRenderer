#pragma once
#include "core/window.hpp"
#include "renderer/renderer.hpp"

class App {
public:
    App();
    ~App();

    int Run();

private:
    Window m_Window;
    Renderer m_Renderer;
};