#ifndef MVK_GRAPHICS_SETTINGS
#define MVK_GRAPHICS_SETTINGS

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "../Shaders/ShadersHelper.h"

namespace mvk {
    const std::vector<vk::DynamicState> DYNAMIC_STATES = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
        vk::DynamicState::ePolygonModeEXT
    };

    class GraphicsSettings {
       public:
        std::vector<vk::PipelineShaderStageCreateInfo> CreateShadersStages(std::vector<vk::ShaderModule> shaders);
        vk::PipelineVertexInputStateCreateInfo CreateVertexInput();
        vk::PipelineInputAssemblyStateCreateInfo CreateInputAssembly();
        vk::PipelineViewportStateCreateInfo CreateViewport();
        vk::PipelineRasterizationStateCreateInfo CreateRasterizer();
        vk::PipelineMultisampleStateCreateInfo CreateMultisampling();
        vk::PipelineColorBlendAttachmentState CreateColorBlend();
        vk::PipelineColorBlendStateCreateInfo CreateColorBlendInfo(vk::PipelineColorBlendAttachmentState& colorblend);
        vk::PipelineDynamicStateCreateInfo CreateDynamicStates();
    };
}

#endif // MVK_GRAPHICS_SETTINGS
