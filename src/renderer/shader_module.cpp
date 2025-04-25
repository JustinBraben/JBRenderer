#include "../stdafx.h"
#include "shader_module.hpp"

ShaderModule::ShaderModule(VulkanContext& context, const std::string& filepath)
    : m_Context(context)
{
    auto code = ReadFile(filepath);
    
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (m_Context.GetDispatchTable().createShaderModule(&createInfo, nullptr, &m_ShaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module");
    }
}

ShaderModule::~ShaderModule() {
    m_Context.GetDispatchTable().destroyShaderModule(m_ShaderModule, nullptr);
}

std::vector<char> ShaderModule::ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
    file.close();

    return buffer;
}