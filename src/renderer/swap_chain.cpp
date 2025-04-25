#include "../stdafx.h"
#include "swap_chain.hpp"

SwapChain::SwapChain(VulkanContext& context)
    : m_Context(context)
{
    Initialize();
}

SwapChain::~SwapChain() {
    Cleanup();
}

void SwapChain::Cleanup() {
    m_Swapchain.destroy_image_views(m_ImageViews);
    vkb::destroy_swapchain(m_Swapchain);
}

void SwapChain::Initialize() {
    vkb::SwapchainBuilder swapchainBuilder{m_Context.GetDevice()};
    auto swapchainRet = swapchainBuilder
        .set_old_swapchain(m_Swapchain)
        .build();
    
    if (!swapchainRet) {
        throw std::runtime_error("Failed to create swapchain: " + 
                               swapchainRet.error().message());
    }
    
    // Clean up old swapchain
    vkb::destroy_swapchain(m_Swapchain);
    
    // Set new swapchain
    m_Swapchain = swapchainRet.value();
    
    // Get images and image views
    auto imagesRet = m_Swapchain.get_images();
    if (!imagesRet) {
        throw std::runtime_error("Failed to get swapchain images");
    }
    m_Images = imagesRet.value();
    
    auto imageViewsRet = m_Swapchain.get_image_views();
    if (!imageViewsRet) {
        throw std::runtime_error("Failed to get swapchain image views");
    }
    m_ImageViews = imageViewsRet.value();
}

void SwapChain::Recreate() {
    Cleanup();
    Initialize();
}