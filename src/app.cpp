#include "stdafx.h"
#include "app.hpp"

App::App()
    : m_Window(1024, 1024, "Vulkan Renderer"),
      m_Renderer(m_Window)
{
}

App::~App() {
    // Cleanup happens in destructors
}

int App::Run() {
    while (!m_Window.ShouldClose()) {
        m_Window.PollEvents();
        
        try {
            m_Renderer.DrawFrame();
        } catch (const std::exception& e) {
            // Log error
            return -1;
        }
    }
    
    m_Renderer.WaitIdle();
    return 0;
}
