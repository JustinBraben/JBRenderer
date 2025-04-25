#pragma once
#include <vulkan/vulkan_core.h>
#include "vulkan_context.hpp"
#include "render_pass.hpp"
#include "shader_module.hpp"
#include "swap_chain.hpp"

class Pipeline {
public:
    Pipeline(VulkanContext& context, RenderPass& renderPass, SwapChain& swapchain);
    ~Pipeline();

    VkPipeline GetHandle() const { return m_Pipeline; }
    VkPipelineLayout GetLayout() const { return m_PipelineLayout; }

private:
    VulkanContext& m_Context;
    RenderPass& m_RenderPass;
    SwapChain& m_Swapchain;
    VkPipelineLayout m_PipelineLayout;
    VkPipeline m_Pipeline;
};