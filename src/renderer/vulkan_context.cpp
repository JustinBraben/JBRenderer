#include "../stdafx.h"
#include "vulkan_context.hpp"

VulkanContext::VulkanContext(Window& window) {
    // Create instance
    vkb::InstanceBuilder instanceBuilder;
    auto instanceRet = instanceBuilder
        .use_default_debug_messenger()
        .request_validation_layers()
        .build();
    
    if (!instanceRet) {
        throw std::runtime_error("Failed to create Vulkan instance: " + 
                               instanceRet.error().message());
    }
    m_Instance = instanceRet.value();
    m_InstanceDispatch = m_Instance.make_table();

    // Create surface
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkResult err = glfwCreateWindowSurface(m_Instance, 
                                          window.GetHandle(), 
                                          nullptr, 
                                          &surface);
    if (err) {
        const char* errorMsg;
        int ret = glfwGetError(&errorMsg);
        throw std::runtime_error("Failed to create window surface");
    }
    m_Surface = surface;

    // Select physical device and create logical device
    vkb::PhysicalDeviceSelector physDeviceSelector(m_Instance);
    auto physDeviceRet = physDeviceSelector
        .set_surface(m_Surface)
        .select();
    
    if (!physDeviceRet) {
        throw std::runtime_error("Failed to select physical device: " + 
                               physDeviceRet.error().message());
    }
    
    vkb::DeviceBuilder deviceBuilder{physDeviceRet.value()};
    auto deviceRet = deviceBuilder.build();
    
    if (!deviceRet) {
        throw std::runtime_error("Failed to create logical device: " + 
                               deviceRet.error().message());
    }
    
    m_Device = deviceRet.value();
    m_DispatchTable = m_Device.make_table();

    // Get queues
    auto graphicsQueueRet = m_Device.get_queue(vkb::QueueType::graphics);
    if (!graphicsQueueRet) {
        throw std::runtime_error("Failed to get graphics queue: " + 
                               graphicsQueueRet.error().message());
    }
    m_GraphicsQueue = graphicsQueueRet.value();

    auto presentQueueRet = m_Device.get_queue(vkb::QueueType::present);
    if (!presentQueueRet) {
        throw std::runtime_error("Failed to get present queue: " + 
                               presentQueueRet.error().message());
    }
    m_PresentQueue = presentQueueRet.value();
}

VulkanContext::~VulkanContext() {
    vkb::destroy_device(m_Device);
    vkb::destroy_surface(m_Instance, m_Surface);
    vkb::destroy_instance(m_Instance);
}

uint32_t VulkanContext::GetGraphicsQueueIndex() const {
    return m_Device.get_queue_index(vkb::QueueType::graphics).value();
}