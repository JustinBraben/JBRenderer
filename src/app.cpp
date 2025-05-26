#include "stdafx.h"
#include "app.hpp"

App::App()
    : m_Window(1024, 1024, "Vulkan Renderer"),
      m_Renderer(m_Window)
{
    m_Camera.type = Camera::CameraType::lookat;
    m_Camera.setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
    m_Camera.setRotation(glm::vec3(0.0f));
    m_Camera.setPerspective(60.0f, (float)m_Window.GetWidth() / (float)m_Window.GetHeight(), 1.0f, 256.0f);
}

App::~App() {
    // Cleanup happens in destructors
}

int App::Run() {
    while (!m_Window.ShouldClose()) {
        m_Window.PollEvents();
        
        try {
            auto tStart = std::chrono::high_resolution_clock::now();
            m_Renderer.DrawFrame();
            auto tEnd = std::chrono::high_resolution_clock::now();
            auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
            frameTimer = (float)tDiff / 1000.0f;
            m_Camera.update(frameTimer);
        } catch (const std::exception& e) {
            // Log error
            return -1;
        }
    }
    
    m_Renderer.WaitIdle();
    return 0;
}
