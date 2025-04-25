#include "../stdafx.h"
#include "renderer.hpp"

Renderer::Renderer(Window& window)
    : m_Window(window),
      m_Context(window),
      m_Swapchain(m_Context),
      m_RenderPass(m_Context, m_Swapchain),
      m_Pipeline(m_Context, m_RenderPass, m_Swapchain),
      m_Framebuffers(m_Context, m_Swapchain, m_RenderPass),
      m_CommandManager(m_Context, m_Swapchain.GetImageCount()),
      m_Synchronization(m_Context, m_Swapchain.GetImageCount())
{
    // Record initial command buffers
    m_CommandManager.RecordCommands(
        m_Swapchain,
        m_RenderPass,
        m_Framebuffers,
        m_Pipeline
    );
}

Renderer::~Renderer() {
    // Resources are cleaned up in reverse order of creation via destructors
}

void Renderer::WaitIdle() {
    m_Context.GetDispatchTable().deviceWaitIdle();
}

int Renderer::RecreateSwapchain() {
    WaitIdle();
    
    // Recreate necessary components
    m_Swapchain.Recreate();
    m_Framebuffers.Recreate();
    m_CommandManager.RecordCommands(
        m_Swapchain,
        m_RenderPass,
        m_Framebuffers,
        m_Pipeline
    );
    
    return 0;
}

void Renderer::DrawFrame() {
    auto& disp = m_Context.GetDispatchTable();
    auto currentFrame = m_Synchronization.GetCurrentFrame();
    auto& inFlightFences = m_Synchronization.GetInFlightFences();
    auto& imageInFlightFences = m_Synchronization.GetImageInFlightFences();
    auto& availableSemaphores = m_Synchronization.GetAvailableSemaphores();
    auto& finishedSemaphores = m_Synchronization.GetFinishedSemaphores();
    
    disp.waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    
    uint32_t imageIndex;
    VkResult result = disp.acquireNextImageKHR(
        m_Swapchain.GetHandle(), 
        UINT64_MAX, 
        availableSemaphores[currentFrame], 
        VK_NULL_HANDLE, 
        &imageIndex
    );
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapchain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image");
    }
    
    if (imageInFlightFences[imageIndex] != VK_NULL_HANDLE) {
        disp.waitForFences(1, &imageInFlightFences[imageIndex], VK_TRUE, UINT64_MAX);
    }
    imageInFlightFences[imageIndex] = inFlightFences[currentFrame];
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkSemaphore waitSemaphores[] = {availableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    
    auto& commandBuffers = m_CommandManager.GetBuffers();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
    
    VkSemaphore signalSemaphores[] = {finishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    disp.resetFences(1, &inFlightFences[currentFrame]);
    
    if (disp.queueSubmit(m_Context.GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer");
    }
    
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    
    VkSwapchainKHR swapchains[] = {m_Swapchain.GetHandle()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    
    result = disp.queuePresentKHR(m_Context.GetPresentQueue(), &presentInfo);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RecreateSwapchain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swapchain image");
    }
    
    m_Synchronization.NextFrame();
}
