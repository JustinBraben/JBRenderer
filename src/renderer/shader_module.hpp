#pragma once
#include <vulkan/vulkan_core.h>
#include <string>
#include <vector>
#include "vulkan_context.hpp"

class ShaderModule {
public:
    ShaderModule(VulkanContext& context, const std::string& filepath);
    ~ShaderModule();

    VkShaderModule GetHandle() const { return m_ShaderModule; }
    static std::vector<char> ReadFile(const std::string& filename);

private:
    VulkanContext& m_Context;
    VkShaderModule m_ShaderModule;
};