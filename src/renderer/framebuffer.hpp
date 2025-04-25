#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "vulkan_context.hpp"
#include "swap_chain.hpp"
#include "render_pass.hpp"

class Framebuffer {
public:
    Framebuffer(VulkanContext& context, SwapChain& swapchain, RenderPass& renderPass);
    ~Framebuffer();

    void Recreate();
    const std::vector<VkFramebuffer>& GetHandles() const { return m_Framebuffers; }

private:
    VulkanContext& m_Context;
    SwapChain& m_Swapchain;
    RenderPass& m_RenderPass;
    std::vector<VkFramebuffer> m_Framebuffers;

    void Cleanup();
    void Initialize();
};