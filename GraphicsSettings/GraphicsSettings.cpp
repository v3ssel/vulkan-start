#include "GraphicsSettings.h"

std::vector<vk::PipelineShaderStageCreateInfo> mvk::GraphicsSettings::CreateShadersStages(std::vector<vk::ShaderModule> shaders) {
    std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;

    vk::PipelineShaderStageCreateInfo vertex_pipeline_info{};
    vertex_pipeline_info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
    vertex_pipeline_info.setStage(vk::ShaderStageFlagBits::eVertex);
    vertex_pipeline_info.setModule(shaders.at(0));
    vertex_pipeline_info.setPName("main");
    shader_stages.push_back(vertex_pipeline_info);

    vk::PipelineShaderStageCreateInfo fragment_pipeline_info{};
    fragment_pipeline_info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
    fragment_pipeline_info.setStage(vk::ShaderStageFlagBits::eFragment);
    fragment_pipeline_info.setModule(shaders.at(1));
    fragment_pipeline_info.setPName("main");
    shader_stages.push_back(fragment_pipeline_info);

    return shader_stages;
}

vk::PipelineVertexInputStateCreateInfo mvk::GraphicsSettings::CreateVertexInput() {
    vk::PipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
    vertex_input_info.setVertexBindingDescriptionCount(0);
    vertex_input_info.setVertexAttributeDescriptionCount(0);

    return vertex_input_info;
}
vk::PipelineInputAssemblyStateCreateInfo mvk::GraphicsSettings::CreateInputAssembly() {
    vk::PipelineInputAssemblyStateCreateInfo input_assembly_info{};
    input_assembly_info.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
    // DRAW TRIANGLES HERE
    input_assembly_info.setTopology(vk::PrimitiveTopology::eTriangleList);
    input_assembly_info.setPrimitiveRestartEnable(VK_FALSE);

    return input_assembly_info;
}

vk::PipelineViewportStateCreateInfo mvk::GraphicsSettings::CreateViewport() {
    vk::PipelineViewportStateCreateInfo viewport_info{};
    viewport_info.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
    viewport_info.setViewportCount(1);
    viewport_info.setScissorCount(1);

    return viewport_info;
}

vk::PipelineRasterizationStateCreateInfo mvk::GraphicsSettings::CreateRasterizer() {
    vk::PipelineRasterizationStateCreateInfo rasterizer_info{};
    rasterizer_info.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
    rasterizer_info.setDepthClampEnable(VK_FALSE);
    rasterizer_info.setRasterizerDiscardEnable(VK_FALSE);
    rasterizer_info.setPolygonMode(vk::PolygonMode::eFill);
    rasterizer_info.setLineWidth(1.0f);
    rasterizer_info.setPolygonMode(vk::PolygonMode::eFill);
    rasterizer_info.setCullMode(vk::CullModeFlagBits::eBack);
    rasterizer_info.setFrontFace(vk::FrontFace::eClockwise);
    rasterizer_info.setDepthBiasEnable(VK_FALSE);
    rasterizer_info.setDepthBiasConstantFactor(0.0f);
    rasterizer_info.setDepthBiasClamp(0.0f);
    rasterizer_info.setDepthBiasSlopeFactor(0.0f);

    return rasterizer_info;
}

vk::PipelineMultisampleStateCreateInfo mvk::GraphicsSettings::CreateMultisampling() {
    vk::PipelineMultisampleStateCreateInfo multisampling_info{};
    multisampling_info.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
    multisampling_info.setSampleShadingEnable(VK_FALSE);
    multisampling_info.setRasterizationSamples(vk::SampleCountFlagBits::e1);
    multisampling_info.setMinSampleShading(1.0f);
    multisampling_info.setPSampleMask(nullptr);
    multisampling_info.setAlphaToCoverageEnable(VK_FALSE);
    multisampling_info.setAlphaToOneEnable(VK_FALSE);

    return multisampling_info;
}

vk::PipelineColorBlendAttachmentState mvk::GraphicsSettings::CreateColorBlend() {
    vk::PipelineColorBlendAttachmentState colorblend{};
    colorblend.setColorWriteMask(vk::ColorComponentFlags(
        vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA
    ));
    
    colorblend.setBlendEnable(VK_TRUE);
    colorblend.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
    colorblend.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
    colorblend.setColorBlendOp(vk::BlendOp::eAdd);

    colorblend.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
    colorblend.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
    colorblend.setAlphaBlendOp(vk::BlendOp::eAdd);

    return colorblend;
}

vk::PipelineColorBlendStateCreateInfo mvk::GraphicsSettings::CreateColorBlendInfo(vk::PipelineColorBlendAttachmentState& colorblend) {
    vk::PipelineColorBlendStateCreateInfo colorblend_info{};
    colorblend_info.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
    colorblend_info.setLogicOpEnable(VK_FALSE);
    colorblend_info.setLogicOp(vk::LogicOp::eCopy);
    colorblend_info.setAttachmentCount(1);
    colorblend_info.setPAttachments(&colorblend);
    colorblend_info.setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});

    return colorblend_info;
}

vk::PipelineDynamicStateCreateInfo mvk::GraphicsSettings::CreateDynamicStates() {
    vk::PipelineDynamicStateCreateInfo dynamic_state_info{};
    dynamic_state_info.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
    dynamic_state_info.setDynamicStateCount(static_cast<uint32_t>(DYNAMIC_STATES.size()));
    dynamic_state_info.setPDynamicStates(DYNAMIC_STATES.data());

    return dynamic_state_info;
}
