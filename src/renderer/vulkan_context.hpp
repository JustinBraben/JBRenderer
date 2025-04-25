#pragma once
#include <vulkan/vulkan_core.h>
#include <VkBootstrap.h>
#include "../core/window.hpp"

class VulkanContext {
public:
    VulkanContext(Window& window);
    ~VulkanContext();

    const vkb::Instance& GetInstance() const { return m_Instance; }
    const vkb::Device& GetDevice() const { return m_Device; }
    const vkb::DispatchTable& GetDispatchTable() const { return m_DispatchTable; }
    const vkb::InstanceDispatchTable& GetInstanceDispatch() const { return m_InstanceDispatch; }
    VkSurfaceKHR GetSurface() const { return m_Surface; }
    VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
    VkQueue GetPresentQueue() const { return m_PresentQueue; }
    uint32_t GetGraphicsQueueIndex() const;

private:
    vkb::Instance m_Instance;
    vkb::InstanceDispatchTable m_InstanceDispatch;
    VkSurfaceKHR m_Surface;
    vkb::Device m_Device;
    vkb::DispatchTable m_DispatchTable;
    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;
};