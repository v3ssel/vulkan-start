#include "../QueueFamilies/QueueFamilies.h"
#include "VulkanManager.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {

    if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        std::cout << "\u001b[31mERROR: " << callback_data->pMessage << '\n';
    else if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        std::cout << "\u001b[33mWARNING: " << callback_data->pMessage << '\n';
    else if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        std::cout << "\u001b[32mINFO: " << callback_data->pMessage << '\n';
    else
        std::cout << "\u001b[0mVERBOSE: " << callback_data->pMessage << '\n';
    
    return VK_FALSE;
}

namespace mvk {
    void VulkanManager::CreateInstance() {
        if (ENABLE_VALIDATION_LAYERS && !vo_.validator.CheckValidationLayersSupport(VALIDATION_LAYERS))
            throw std::runtime_error("Requested validation layers unavailable.");

        vk::ApplicationInfo app_info;
        app_info.sType = vk::StructureType::eApplicationInfo;
        app_info.setPApplicationName("Vulkan DisplayWindow");
        app_info.setApplicationVersion(VK_MAKE_API_VERSION(0, 1, 0, 0));
        app_info.setPEngineName("No Engine");
        app_info.setEngineVersion(VK_MAKE_API_VERSION(0, 1, 0, 0));
        app_info.setApiVersion(VK_API_VERSION_1_0);

        vk::InstanceCreateInfo create_info{};
        create_info.sType = vk::StructureType::eInstanceCreateInfo;
        create_info.setPApplicationInfo(&app_info);

        auto requirment_extensions = vo_.validator.GetRequirmentInstanceExtension(ENABLE_VALIDATION_LAYERS);
        vo_.validator.CheckRequestedExtensions(requirment_extensions);
        create_info.setEnabledExtensionCount(requirment_extensions.size());
        create_info.setPpEnabledExtensionNames(requirment_extensions.data());

        vk::DebugUtilsMessengerCreateInfoEXT debug_info{};
        if (ENABLE_VALIDATION_LAYERS) {
            create_info.setEnabledLayerCount(static_cast<uint32_t>(VALIDATION_LAYERS.size()));
            create_info.setPpEnabledLayerNames(VALIDATION_LAYERS.data());

            FillDebugInfo(debug_info);
            create_info.setPNext((vk::DebugUtilsMessengerCreateInfoEXT *)&debug_info);
        } else {
            create_info.setEnabledLayerCount(0);
            create_info.setPNext(nullptr);
        }

        if (vk::createInstance(&create_info, nullptr, &vo_.instance) != vk::Result::eSuccess) {
            throw std::runtime_error("Cannot create instance.");
        }
    }

    void VulkanManager::SetupDebug() {
        if (!ENABLE_VALIDATION_LAYERS) return;

        vk::DebugUtilsMessengerCreateInfoEXT debug_info;
        FillDebugInfo(debug_info);

        vo_.debug_messenger = vo_.instance.createDebugUtilsMessengerEXT(debug_info, nullptr, vk::DispatchLoaderDynamic(vo_.instance, vkGetInstanceProcAddr));
    }

    void VulkanManager::CreateSurface(GLFWwindow *window) {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(vo_.instance, window, nullptr, &surface) != VK_SUCCESS)
            throw std::runtime_error("Failed to create window surface.");
        vo_.surface = vk::SurfaceKHR(surface);
    }

    void VulkanManager::TakeVideocard() {
        auto devices = vo_.instance.enumeratePhysicalDevices();

        if (devices.size() == 0) throw std::runtime_error("Supported GPU not found.");

        for (auto &device : devices) {
            if (vo_.validator.CheckVideocard(device, vo_.surface, DEVICE_REQUIRED_EXTENSIONS)) {
                vo_.physical_device = device;
                break;
            }
        }
        if (static_cast<VkPhysicalDevice>(vo_.physical_device) == nullptr)
            throw std::runtime_error("Supported GPU not found.");
    }

    void VulkanManager::CreateLogicalDevice() {
        QueueFamilies indices = QueueFamilies::FindQueueFamily(vo_.physical_device, vo_.surface);

        std::vector<vk::DeviceQueueCreateInfo> device_queue_infos{};
        std::set<uint32_t> unique_families = {indices.graphics_family_.value(), indices.present_family_.value()};

        float q = 1.0f;
        for (auto &queue_family : unique_families) {
            vk::DeviceQueueCreateInfo logical_device_queue_info{};
            logical_device_queue_info.sType = vk::StructureType::eDeviceQueueCreateInfo;
            logical_device_queue_info.setQueueFamilyIndex(indices.graphics_family_.value());
            logical_device_queue_info.setQueueCount(1);
            logical_device_queue_info.setQueuePriorities(q);
            device_queue_infos.push_back(logical_device_queue_info);
        }

        vk::DeviceCreateInfo logical_device_info{};
        logical_device_info.sType = vk::StructureType::eDeviceCreateInfo;
        logical_device_info.setQueueCreateInfoCount(device_queue_infos.size());
        logical_device_info.setPQueueCreateInfos(device_queue_infos.data());
        logical_device_info.setEnabledExtensionCount(static_cast<uint32_t>(DEVICE_REQUIRED_EXTENSIONS.size()));
        logical_device_info.setPpEnabledExtensionNames(DEVICE_REQUIRED_EXTENSIONS.data());

        vk::PhysicalDeviceFeatures features{};
        logical_device_info.setPEnabledFeatures(&features);

        vo_.logical_device = vo_.physical_device.createDevice(logical_device_info);
        vo_.graphics_queue = vo_.logical_device.getQueue(indices.graphics_family_.value(), 0);
        vo_.present_queue = vo_.logical_device.getQueue(indices.present_family_.value(), 0);
    }

    void VulkanManager::CreateSwapChain(GLFWwindow *window) {
        SwapChainDetails sc_details(vo_.physical_device, vo_.surface);

        vk::SurfaceFormatKHR format = sc_details.ChooseSwapSurfaceFormat();
        vk::PresentModeKHR present_mode = sc_details.ChooseSwapPresentMode();
        vk::Extent2D extent = sc_details.ChooseSwapExtent(window);

        uint32_t image_count = sc_details.capabilities_.minImageCount + 1;
        if (sc_details.capabilities_.maxImageCount > 0 &&
            image_count > sc_details.capabilities_.maxImageCount)
            image_count = sc_details.capabilities_.maxImageCount;

        vk::SwapchainCreateInfoKHR sc_info{};
        sc_info.sType = vk::StructureType::eSwapchainCreateInfoKHR;
        sc_info.setSurface(vo_.surface);
        sc_info.setMinImageCount(image_count);
        sc_info.setImageFormat(format.format);
        sc_info.setImageColorSpace(format.colorSpace);
        sc_info.setImageExtent(extent);
        sc_info.setImageArrayLayers(1);
        sc_info.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

        QueueFamilies families = QueueFamilies::FindQueueFamily(vo_.physical_device, vo_.surface);
        uint32_t families_indices[] = {families.graphics_family_.value(), families.present_family_.value()};
        if (families.graphics_family_.value() != families.present_family_.value()) {
            sc_info.setImageSharingMode(vk::SharingMode::eConcurrent);
            sc_info.setQueueFamilyIndexCount(2);
            sc_info.setPQueueFamilyIndices(families_indices);
        } else {
            sc_info.setImageSharingMode(vk::SharingMode::eExclusive);
            sc_info.setQueueFamilyIndexCount(0);
            sc_info.setPQueueFamilyIndices(nullptr);
        }

        sc_info.setPreTransform(sc_details.capabilities_.currentTransform);
        sc_info.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        sc_info.setPresentMode(present_mode);
        sc_info.setClipped(VK_TRUE);
        sc_info.setOldSwapchain(VK_NULL_HANDLE);

        vo_.swapchain = vo_.logical_device.createSwapchainKHR(sc_info);

        vo_.swapchain_images = vo_.logical_device.getSwapchainImagesKHR(vo_.swapchain);
        vo_.sc_format = format.format;
        vo_.sc_extent = extent;
    }

    void VulkanManager::CreateImageView() {
        vo_.image_views.resize(vo_.swapchain_images.size());

        for (size_t i = 0; i < vo_.swapchain_images.size(); ++i) {
            vk::ImageViewCreateInfo image_info{};
            image_info.sType = vk::StructureType::eImageViewCreateInfo;
            image_info.setImage(vo_.swapchain_images[i]);
            image_info.setViewType(vk::ImageViewType::e2D);
            image_info.setFormat(vo_.sc_format);
            image_info.setComponents(
                vk::ComponentMapping(
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity
                )
            );
            image_info.setSubresourceRange(
                vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
            );
            vo_.image_views[i] = vo_.logical_device.createImageView(image_info);
        }
    }

    void VulkanManager::CreateGraphicsPipeline() {
        vk::ShaderModuleCreateInfo vertex_info = ShadersHelper::LoadVertexShader();
        vk::ShaderModuleCreateInfo fragment_info = ShadersHelper::LoadFragmentShader();
        
        vk::ShaderModule vertex_module = vo_.logical_device.createShaderModule(vertex_info);
        vk::ShaderModule fragment_module = vo_.logical_device.createShaderModule(fragment_info);
        std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;

        vk::PipelineShaderStageCreateInfo vertex_pipeline_info{};
        vertex_pipeline_info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        vertex_pipeline_info.setStage(vk::ShaderStageFlagBits::eVertex);
        vertex_pipeline_info.setModule(vertex_module);
        vertex_pipeline_info.setPName("main");
        shader_stages.push_back(vertex_pipeline_info);

        vk::PipelineShaderStageCreateInfo fragment_pipeline_info{};
        fragment_pipeline_info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        fragment_pipeline_info.setStage(vk::ShaderStageFlagBits::eFragment);
        fragment_pipeline_info.setModule(fragment_module);
        fragment_pipeline_info.setPName("main");
        shader_stages.push_back(fragment_pipeline_info);


        std::vector<vk::DynamicState> dynamic_states = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };

        vk::PipelineDynamicStateCreateInfo dynamic_state_info{};
        dynamic_state_info.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
        dynamic_state_info.setDynamicStateCount(static_cast<uint32_t>(dynamic_states.size()));
        dynamic_state_info.setPDynamicStates(dynamic_states.data());


        vk::PipelineVertexInputStateCreateInfo vertex_input_info{};
        vertex_input_info.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
        vertex_input_info.setVertexBindingDescriptionCount(0);
        vertex_input_info.setVertexAttributeDescriptionCount(0);


        // DRAW TRIANGLES HERE
        vk::PipelineInputAssemblyStateCreateInfo input_assembly_info{};
        input_assembly_info.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
        input_assembly_info.setTopology(vk::PrimitiveTopology::eTriangleList);
        input_assembly_info.setPrimitiveRestartEnable(VK_FALSE);


        vk::Viewport viewport{};
        viewport.setX(0.0f);
        viewport.setY(0.0f);
        viewport.setWidth(vo_.sc_extent.width);
        viewport.setHeight(vo_.sc_extent.height);
        viewport.setMinDepth(0.0f);
        viewport.setMaxDepth(1.0f);

        vk::Rect2D scissor{};
        scissor.setOffset(vk::Offset2D((double)0, (double)0));
        scissor.setExtent(vo_.sc_extent);


        vk::PipelineRasterizationStateCreateInfo rasterizer_info{};
        rasterizer_info.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
        rasterizer_info.setDepthClampEnable(VK_FALSE);
        rasterizer_info.setRasterizerDiscardEnable(VK_FALSE);
        rasterizer_info.setPolygonMode(vk::PolygonMode::eFill);
        rasterizer_info.setLineWidth(1.0f);
        rasterizer_info.setCullMode(vk::CullModeFlagBits::eBack);
        rasterizer_info.setFrontFace(vk::FrontFace::eClockwise);
        rasterizer_info.setDepthBiasEnable(VK_FALSE);
        rasterizer_info.setDepthBiasConstantFactor(0.0f);
        rasterizer_info.setDepthBiasClamp(0.0f);
        rasterizer_info.setDepthBiasSlopeFactor(0.0f);


        // multisampling is off
        vk::PipelineMultisampleStateCreateInfo multisampling_info{};
        multisampling_info.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
        multisampling_info.setSampleShadingEnable(VK_FALSE);
        multisampling_info.setRasterizationSamples(vk::SampleCountFlagBits::e1);
        multisampling_info.setMinSampleShading(1.0f);
        multisampling_info.setPSampleMask(nullptr);
        multisampling_info.setAlphaToCoverageEnable(VK_FALSE);
        multisampling_info.setAlphaToOneEnable(VK_FALSE);


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


        vk::PipelineLayoutCreateInfo layout_info{};
        layout_info.sType = vk::StructureType::ePipelineLayoutCreateInfo;
        layout_info.setSetLayoutCount(0);
        layout_info.setPushConstantRangeCount(0);
        
        vo_.layout = vo_.logical_device.createPipelineLayout(layout_info);
        
        vo_.logical_device.destroyShaderModule(vertex_module);
        vo_.logical_device.destroyShaderModule(fragment_module);
    }

    void VulkanManager::DestroyEverything() {
        if (ENABLE_VALIDATION_LAYERS)
            vo_.instance.destroyDebugUtilsMessengerEXT(vo_.debug_messenger, nullptr, vk::DispatchLoaderDynamic(vo_.instance, vkGetInstanceProcAddr));
        
        vo_.logical_device.destroyPipelineLayout(vo_.layout);
        for (auto image_view : vo_.image_views) {
            vo_.logical_device.destroyImageView(image_view);
        }
        vo_.logical_device.destroySwapchainKHR(vo_.swapchain);
        vo_.logical_device.destroy();
        vo_.instance.destroySurfaceKHR(vo_.surface);
        vo_.instance.destroy();
    }

    void VulkanManager::FillDebugInfo(vk::DebugUtilsMessengerCreateInfoEXT& debug_info) {
        debug_info.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
        debug_info.setMessageSeverity(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo    |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        );

        debug_info.setMessageType(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
        );

        debug_info.setPfnUserCallback(DebugCallback);
        debug_info.setPUserData(nullptr);
    }

    void VulkanManager::PrintLoadedData() {
        auto extensions = vk::enumerateInstanceExtensionProperties();
        std::cout << "\u001b[36mLOADED EXTENSIONS:\n";
        for (auto &ext : extensions)
            std::cout << '\t' << ext.extensionName << '\n';


        auto layers = vk::enumerateInstanceLayerProperties();
        std::cout << "LAYERS:\n";
        for (auto &layer : layers)
            std::cout << '\t' << layer.layerName << "\n";

        auto devices = vo_.instance.enumeratePhysicalDevices();
        std::cout << "DEVICES:\n";
        for (auto &device : devices) {
            std::cout << "\t" << device.getProperties().deviceName << "\n\tEXTENSIONS:\n";
            // for (auto& device_ext : device.enumerateDeviceExtensionProperties())
            //     std::cout << "\t\t" << device_ext.extensionName << "\n";

            // std::cout << "\t" << "LAYERS:\n";
            // for (auto& layer : device.enumerateDeviceLayerProperties())
            //     std::cout << "\t\t" << layer.layerName << "\n";

            // std::cout << "\t" << "QUEUES:\n";
            // for (auto &queue : device.getQueueFamilyProperties())
            //     std::cout << "\t\t" << queue.queueCount << "\n";
        }
        std::cout << "\u001b[0m";
    }
}
