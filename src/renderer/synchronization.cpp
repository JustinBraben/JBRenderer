#include "../stdafx.h"
#include "synchronization.hpp"

Synchronization::Synchronization(VulkanContext& context, uint32_t swapchainImageCount)
    : m_Context(context)
{
    m_AvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_FinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    m_ImageInFlightFences.resize(swapchainImageCount, VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    auto& disp = m_Context.GetDispatchTable();
    
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (disp.createSemaphore(&semaphoreInfo, nullptr, &m_AvailableSemaphores[i]) != VK_SUCCESS ||
            disp.createSemaphore(&semaphoreInfo, nullptr, &m_FinishedSemaphores[i]) != VK_SUCCESS ||
            disp.createFence(&fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create synchronization objects");
        }
    }
}

Synchronization::~Synchronization() {
    auto& disp = m_Context.GetDispatchTable();
    
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        disp.destroySemaphore(m_AvailableSemaphores[i], nullptr);
        disp.destroySemaphore(m_FinishedSemaphores[i], nullptr);
        disp.destroyFence(m_InFlightFences[i], nullptr);
    }
}