#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "vulkan_context.hpp"

const int MAX_FRAMES_IN_FLIGHT = 2;

class Synchronization {
public:
    Synchronization(VulkanContext& context, uint32_t swapchainImageCount);
    ~Synchronization();

    const std::vector<VkSemaphore>& GetAvailableSemaphores() const { return m_AvailableSemaphores; }
    const std::vector<VkSemaphore>& GetFinishedSemaphores() const { return m_FinishedSemaphores; }
    const std::vector<VkFence>& GetInFlightFences() const { return m_InFlightFences; }
    std::vector<VkFence>& GetImageInFlightFences() { return m_ImageInFlightFences; }
    
    size_t GetCurrentFrame() const { return m_CurrentFrame; }
    void NextFrame() { m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT; }

private:
    VulkanContext& m_Context;
    std::vector<VkSemaphore> m_AvailableSemaphores;
    std::vector<VkSemaphore> m_FinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;
    std::vector<VkFence> m_ImageInFlightFences;
    size_t m_CurrentFrame = 0;
};