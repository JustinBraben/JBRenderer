#include "../stdafx.h"
#include "command_manager.hpp"

CommandManager::CommandManager(VulkanContext& context, uint32_t bufferCount)
    : m_Context(context)
{
    Initialize(bufferCount);
}

CommandManager::~CommandManager() {
    Cleanup();
}

void CommandManager::Cleanup() {
    auto& disp = m_Context.GetDispatchTable();
    disp.destroyCommandPool(m_CommandPool, nullptr);
}

void CommandManager::Initialize(uint32_t bufferCount) {
    // Create command pool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = m_Context.GetGraphicsQueueIndex();

    if (m_Context.GetDispatchTable().createCommandPool(&poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }

    // Allocate command buffers
    m_CommandBuffers.resize(bufferCount);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

    if (m_Context.GetDispatchTable().allocateCommandBuffers(&allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers");
    }
}

void CommandManager::RecordCommands(
    SwapChain& swapchain,
    RenderPass& renderPass,
    Framebuffer& framebuffers,
    Pipeline& pipeline
) {
    auto& disp = m_Context.GetDispatchTable();
    
    for (size_t i = 0; i < m_CommandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (disp.beginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass.GetHandle();
        renderPassInfo.framebuffer = framebuffers.GetHandles()[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain.GetExtent();

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchain.GetExtent().width);
        viewport.height = static_cast<float>(swapchain.GetExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapchain.GetExtent();

        disp.cmdSetViewport(m_CommandBuffers[i], 0, 1, &viewport);
        disp.cmdSetScissor(m_CommandBuffers[i], 0, 1, &scissor);
        disp.cmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        disp.cmdBindPipeline(m_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetHandle());
        disp.cmdDraw(m_CommandBuffers[i], 3, 1, 0, 0);
        disp.cmdEndRenderPass(m_CommandBuffers[i]);

        if (disp.endCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer");
        }
    }
}