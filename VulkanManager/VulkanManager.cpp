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

    void VulkanManager::CreateRenderPass() {
        vk::AttachmentDescription color_attachment{};
        color_attachment.setFormat(vo_.sc_format);
        color_attachment.setSamples(vk::SampleCountFlagBits::e1);
        color_attachment.setLoadOp(vk::AttachmentLoadOp::eClear);
        color_attachment.setStoreOp(vk::AttachmentStoreOp::eStore);
        color_attachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
        color_attachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        color_attachment.setInitialLayout(vk::ImageLayout::eUndefined);
        color_attachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

        vk::AttachmentReference color_attachment_ref{};
        color_attachment_ref.setAttachment(0);
        color_attachment_ref.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        vk::SubpassDescription subpass{};
        subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
        subpass.setColorAttachmentCount(1);
        subpass.setPColorAttachments(&color_attachment_ref);

        vk::RenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = vk::StructureType::eRenderPassCreateInfo;
        render_pass_info.setAttachmentCount(1);
        render_pass_info.setPAttachments(&color_attachment);
        render_pass_info.setSubpassCount(1);
        render_pass_info.setPSubpasses(&subpass);

        vo_.render_pass = vo_.logical_device.createRenderPass(render_pass_info);
    }

    void VulkanManager::CreateGraphicsPipeline() {
        vk::ShaderModuleCreateInfo vertex_info = ShadersHelper::LoadVertexShader();
        vk::ShaderModuleCreateInfo fragment_info = ShadersHelper::LoadFragmentShader();
        
        vk::ShaderModule vertex_module = vo_.logical_device.createShaderModule(vertex_info);
        vk::ShaderModule fragment_module = vo_.logical_device.createShaderModule(fragment_info);


        mvk::GraphicsSettings graphics_settings;
        auto shader_stages = graphics_settings.CreateShadersStages({vertex_module, fragment_module});
        auto vertex_input_info = graphics_settings.CreateVertexInput();
        auto input_assembly_info = graphics_settings.CreateInputAssembly();
        auto viewport_info = graphics_settings.CreateViewport();
        auto rasterizer_info = graphics_settings.CreateRasterizer();
        auto multisampling_info = graphics_settings.CreateMultisampling();
        auto colorblend = graphics_settings.CreateColorBlend();
        auto colorblend_info = graphics_settings.CreateColorBlendInfo(colorblend);
        auto dynamic_state_info = graphics_settings.CreateDynamicStates();


        vk::PipelineLayoutCreateInfo layout_info{};
        layout_info.sType = vk::StructureType::ePipelineLayoutCreateInfo;
        layout_info.setSetLayoutCount(0);
        layout_info.setPushConstantRangeCount(0);
        vo_.layout = vo_.logical_device.createPipelineLayout(layout_info);
        

        vk::GraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
        pipeline_info.setStageCount(shader_stages.size());
        pipeline_info.setStages(shader_stages);

        pipeline_info.setPVertexInputState(&vertex_input_info);
        pipeline_info.setPInputAssemblyState(&input_assembly_info);
        pipeline_info.setPViewportState(&viewport_info);
        pipeline_info.setPRasterizationState(&rasterizer_info);
        pipeline_info.setPMultisampleState(&multisampling_info);
        pipeline_info.setPDepthStencilState(nullptr);
        pipeline_info.setPDynamicState(&dynamic_state_info);
        pipeline_info.setPColorBlendState(&colorblend_info);
        pipeline_info.setLayout(vo_.layout);
        pipeline_info.setRenderPass(vo_.render_pass);
        pipeline_info.setSubpass(0);
        pipeline_info.setBasePipelineHandle(VK_NULL_HANDLE);
        pipeline_info.setBasePipelineIndex(-1);

        auto res = vo_.logical_device.createGraphicsPipeline(VK_NULL_HANDLE, pipeline_info);
        if (res.result != vk::Result::eSuccess)
            throw std::runtime_error("Cannot create pipeline.");
        vo_.pipeline = res.value;


        vo_.logical_device.destroyShaderModule(vertex_module);
        vo_.logical_device.destroyShaderModule(fragment_module);
    }

    void VulkanManager::CreateFramebuffers() {
        vo_.framebuffers.resize(vo_.image_views.size());
        for (size_t i = 0; i < vo_.image_views.size(); ++i) {
            vk::ImageView attachments[] = {vo_.image_views[i]};

            vk::FramebufferCreateInfo framebuffer_info{};
            framebuffer_info.sType = vk::StructureType::eFramebufferCreateInfo;
            framebuffer_info.setRenderPass(vo_.render_pass);
            framebuffer_info.setAttachmentCount(1);
            framebuffer_info.setPAttachments(attachments);
            framebuffer_info.setWidth(vo_.sc_extent.width);
            framebuffer_info.setHeight(vo_.sc_extent.height);
            framebuffer_info.setLayers(1);

            vo_.framebuffers[i] = vo_.logical_device.createFramebuffer(framebuffer_info);
        }
    }

    void VulkanManager::CreateCommandPool() {
        QueueFamilies queue = QueueFamilies::FindQueueFamily(vo_.physical_device, vo_.surface);
        vk::CommandPoolCreateInfo cmd_pool_info{};
        cmd_pool_info.sType = vk::StructureType::eCommandPoolCreateInfo;
        cmd_pool_info.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        cmd_pool_info.setQueueFamilyIndex(queue.graphics_family_.value());

        vo_.command_pool = vo_.logical_device.createCommandPool(cmd_pool_info);
    }

    void VulkanManager::CreateCommandBuffer() {
        vk::CommandBufferAllocateInfo cmd_buff_ainfo{};
        cmd_buff_ainfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
        cmd_buff_ainfo.setCommandPool(vo_.command_pool);
        cmd_buff_ainfo.setLevel(vk::CommandBufferLevel::ePrimary);
        cmd_buff_ainfo.setCommandBufferCount(1);

        vo_.command_buffer = vo_.logical_device.allocateCommandBuffers(cmd_buff_ainfo)[0];
    }

    void VulkanManager::DestroyEverything() {
        vo_.logical_device.destroyCommandPool(vo_.command_pool);

        for (auto framebuffer : vo_.framebuffers)
            vo_.logical_device.destroyFramebuffer(framebuffer);

        vo_.logical_device.destroyPipeline(vo_.pipeline);
        vo_.logical_device.destroyPipelineLayout(vo_.layout);
        vo_.logical_device.destroyRenderPass(vo_.render_pass);

        for (auto image_view : vo_.image_views)
            vo_.logical_device.destroyImageView(image_view);

        if (ENABLE_VALIDATION_LAYERS)
            vo_.instance.destroyDebugUtilsMessengerEXT(vo_.debug_messenger, nullptr, vk::DispatchLoaderDynamic(vo_.instance, vkGetInstanceProcAddr));

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

    void VulkanManager::RecordCommandBuffer(vk::CommandBuffer command_buffer, uint32_t image_index) {
        vk::CommandBufferBeginInfo begin_info{};
        begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
        begin_info.setPInheritanceInfo(nullptr);
        
        command_buffer.begin(begin_info);

        vk::RenderPassBeginInfo render_pass_begin_info{};
        render_pass_begin_info.sType = vk::StructureType::eRenderPassBeginInfo;
        render_pass_begin_info.setRenderPass(vo_.render_pass);
        render_pass_begin_info.setFramebuffer(vo_.framebuffers[image_index]);
        render_pass_begin_info.setRenderArea(vk::Rect2D({0, 0}, vo_.sc_extent));

        vk::ClearValue clear_color(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f));
        render_pass_begin_info.setPClearValues(&clear_color);
        render_pass_begin_info.setClearValueCount(1);
        command_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
        command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, vo_.pipeline);

        vk::Viewport viewport{};
        viewport.setX(0.0f);
        viewport.setY(0.0f);
        viewport.setWidth(static_cast<float>(vo_.sc_extent.width));
        viewport.setHeight(static_cast<float>(vo_.sc_extent.height));
        viewport.setMinDepth(0.0f);
        viewport.setMaxDepth(1.0f);
        command_buffer.setViewport(0, 1, &viewport);

        vk::Rect2D scissor{};
        scissor.setOffset(vk::Offset2D((double)0, (double)0));
        scissor.setExtent(vo_.sc_extent);
        command_buffer.setScissor(0, 1, &scissor);

        command_buffer.draw(3, 1, 0, 0);

        command_buffer.endRenderPass();

        command_buffer.end();
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
