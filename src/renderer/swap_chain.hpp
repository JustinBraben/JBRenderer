#pragma once
#include <vulkan/vulkan_core.h>
#include <VkBootstrap.h>
#include <vector>
#include "vulkan_context.hpp"

class SwapChain {
public:
    SwapChain(VulkanContext& context);
    ~SwapChain();

    void Recreate();
    
    const vkb::Swapchain& GetHandle() const { return m_Swapchain; }
    const std::vector<VkImage>& GetImages() const { return m_Images; }
    const std::vector<VkImageView>& GetImageViews() const { return m_ImageViews; }
    VkFormat GetImageFormat() const { return m_Swapchain.image_format; }
    VkExtent2D GetExtent() const { return m_Swapchain.extent; }
    uint32_t GetImageCount() const { return m_Swapchain.image_count; }

private:
    VulkanContext& m_Context;
    vkb::Swapchain m_Swapchain;
    std::vector<VkImage> m_Images;
    std::vector<VkImageView> m_ImageViews;
    
    void Cleanup();
    void Initialize();
};