#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "vulkan_context.hpp"
#include "framebuffer.hpp"
#include "render_pass.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"

class CommandManager {
public:
    CommandManager(VulkanContext& context, uint32_t bufferCount);
    ~CommandManager();

    void RecordCommands(
        SwapChain& swapchain,
        RenderPass& renderPass,
        Framebuffer& framebuffers,
        Pipeline& pipeline
    );

    VkCommandPool GetPool() const { return m_CommandPool; }
    const std::vector<VkCommandBuffer>& GetBuffers() const { return m_CommandBuffers; }

private:
    VulkanContext& m_Context;
    VkCommandPool m_CommandPool;
    std::vector<VkCommandBuffer> m_CommandBuffers;

    void Cleanup();
    void Initialize(uint32_t bufferCount);
};
