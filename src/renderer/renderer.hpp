#pragma once

#include "../stdafx.h"

#pragma once
#include "vulkan_context.hpp"
#include "swap_chain.hpp"
#include "render_pass.hpp"
#include "shader_module.hpp"
#include "pipeline.hpp"
#include "framebuffer.hpp"
#include "command_manager.hpp"
#include "synchronization.hpp"

class Renderer {
public:
    Renderer(Window& window);
    ~Renderer();

    void DrawFrame();
    void WaitIdle();

private:
    Window& m_Window;
    VulkanContext m_Context;
    SwapChain m_Swapchain;
    RenderPass m_RenderPass;
    Pipeline m_Pipeline;
    Framebuffer m_Framebuffers;
    CommandManager m_CommandManager;
    Synchronization m_Synchronization;

    int RecreateSwapchain();
};