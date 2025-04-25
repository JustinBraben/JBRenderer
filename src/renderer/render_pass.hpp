#pragma once
#include <vulkan/vulkan_core.h>
#include "vulkan_context.hpp"
#include "swap_chain.hpp"

class RenderPass {
public:
    RenderPass(VulkanContext& context, SwapChain& swapchain);
    ~RenderPass();

    VkRenderPass GetHandle() const { return m_RenderPass; }

private:
    VulkanContext& m_Context;
    SwapChain& m_Swapchain;
    VkRenderPass m_RenderPass;
};