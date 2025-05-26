#pragma once
#include "stdafx.h"
#include "core/window.hpp"
#include "renderer/renderer.hpp"
#include "scene/camera.hpp"

class App {
public:
    App();
    ~App();

    int Run();

    // high performance timer (if available)
    float frameTimer = 1.0f;

protected:
    // Frame counter to display fps
    uint32_t frameCounter = 0;
    uint32_t lastFPS = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp, tPrevEnd;

private:
    Window m_Window;
    Renderer m_Renderer;
    Camera m_Camera;
};