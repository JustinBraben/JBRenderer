#include "../stdafx.h"
#include "framebuffer.hpp"

Framebuffer::Framebuffer(VulkanContext& context, SwapChain& swapchain, RenderPass& renderPass)
    : m_Context(context), m_Swapchain(swapchain), m_RenderPass(renderPass)
{
    Initialize();
}

Framebuffer::~Framebuffer() {
    Cleanup();
}

void Framebuffer::Cleanup() {
    auto& disp = m_Context.GetDispatchTable();
    for (auto framebuffer : m_Framebuffers) {
        disp.destroyFramebuffer(framebuffer, nullptr);
    }
}

void Framebuffer::Initialize() {
    auto& imageViews = m_Swapchain.GetImageViews();
    m_Framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); i++) {
        VkImageView attachments[] = { imageViews[i] };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass.GetHandle();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_Swapchain.GetExtent().width;
        framebufferInfo.height = m_Swapchain.GetExtent().height;
        framebufferInfo.layers = 1;

        if (m_Context.GetDispatchTable().createFramebuffer(&framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer");
        }
    }
}

void Framebuffer::Recreate() {
    Cleanup();
    Initialize();
}