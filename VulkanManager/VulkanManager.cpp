#include "../QueueFamilies/QueueFamilies.h"
#include "VulkanManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {

    if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        std::cout << "\u001b[31mERROR: " << callback_data->pMessage << "\u001b[0m\n";
    else if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        std::cout << "\u001b[33mWARNING: " << callback_data->pMessage << "\u001b[0m\n";
    else if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        std::cout << "\u001b[32mINFO: " << callback_data->pMessage << "\u001b[0m\n";
    else
        std::cout << "\u001b[0mVERBOSE: " << callback_data->pMessage << "\u001b[0m\n";
    
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
        app_info.setApiVersion(VK_API_VERSION_1_3);

        vk::InstanceCreateInfo create_info{};
        create_info.sType = vk::StructureType::eInstanceCreateInfo;
        create_info.setPApplicationInfo(&app_info);

        auto requirment_extensions = vo_.validator.SetRequirmentInstanceExtension(ENABLE_VALIDATION_LAYERS, INSTANCE_REQUIRED_EXTENSIONS);
        vo_.validator.CheckRequestedInstanceExtensions(requirment_extensions);
        create_info.setEnabledExtensionCount(requirment_extensions.size());
        create_info.setPpEnabledExtensionNames(requirment_extensions.data());

        vk::DebugUtilsMessengerCreateInfoEXT debug_info{};
        if (ENABLE_VALIDATION_LAYERS) {
            create_info.setEnabledLayerCount(static_cast<uint32_t>(VALIDATION_LAYERS.size()));
            create_info.setPpEnabledLayerNames(VALIDATION_LAYERS.data());

            
            vk::ValidationFeatureEnableEXT validation_features_enables[] = { vk::ValidationFeatureEnableEXT::eBestPractices };

            vk::ValidationFeaturesEXT validation_features{};
            validation_features.sType = vk::StructureType::eValidationFeaturesEXT;
            validation_features.setEnabledValidationFeatureCount(1);
            validation_features.setPEnabledValidationFeatures(validation_features_enables);

            FillDebugInfo(debug_info);
            validation_features.setPNext((vk::DebugUtilsMessengerCreateInfoEXT *)&debug_info);
            
            create_info.setPNext((vk::ValidationFeaturesEXT *)&validation_features);
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
        window_ = window;
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
        features.setFillModeNonSolid(VK_TRUE);
        features.setSamplerAnisotropy(VK_TRUE);
        logical_device_info.setPEnabledFeatures(&features);

        vk::PhysicalDeviceExtendedDynamicState3FeaturesEXT extended_features{};
        extended_features.sType = vk::StructureType::ePhysicalDeviceExtendedDynamicState3FeaturesEXT;
        extended_features.setExtendedDynamicState3PolygonMode(VK_TRUE);
        logical_device_info.setPNext(&extended_features);

        vo_.logical_device = vo_.physical_device.createDevice(logical_device_info);
        vo_.graphics_queue = vo_.logical_device.getQueue(indices.graphics_family_.value(), 0);
        vo_.present_queue = vo_.logical_device.getQueue(indices.present_family_.value(), 0);
    }

    void VulkanManager::CreateSwapChain(bool prev) {
        SwapChainDetails sc_details(vo_.physical_device, vo_.surface);

        vk::SurfaceFormatKHR format = sc_details.ChooseSwapSurfaceFormat();
        vk::PresentModeKHR present_mode = sc_details.ChooseSwapPresentMode();
        vk::Extent2D extent = sc_details.ChooseSwapExtent(window_);

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

        vk::SwapchainKHR old_sc = vo_.swapchain;
        sc_info.setOldSwapchain(prev ? old_sc : VK_NULL_HANDLE);

        vo_.swapchain = vo_.logical_device.createSwapchainKHR(sc_info);
        vo_.swapchain_images = vo_.logical_device.getSwapchainImagesKHR(vo_.swapchain);
        vo_.sc_format = format.format;
        vo_.sc_extent = extent;

        if (prev)
            vo_.logical_device.destroySwapchainKHR(old_sc);
    }

    void VulkanManager::RecreateSwapChain() {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window_, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window_, &width, &height);
            glfwWaitEvents();
        }

        vo_.logical_device.waitIdle();

        DestroySwapchainImages();

        CreateSwapChain(&vo_.swapchain);
        CreateImageViews();
        CreateFramebuffers();
    }

    void VulkanManager::CreateImageViews() {
        vo_.image_views.resize(vo_.swapchain_images.size());

        for (size_t i = 0; i < vo_.swapchain_images.size(); ++i) {
            vo_.image_views[i] = CreateImageView(vo_.swapchain_images[i], vo_.sc_format);
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

        vk::SubpassDependency dependency{};
        dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
        dependency.setDstSubpass(0);
        dependency.setSrcAccessMask(vk::AccessFlagBits::eNone);
        dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
        dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);

        vk::RenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = vk::StructureType::eRenderPassCreateInfo;
        render_pass_info.setAttachmentCount(1);
        render_pass_info.setPAttachments(&color_attachment);
        render_pass_info.setSubpassCount(1);
        render_pass_info.setPSubpasses(&subpass);
        render_pass_info.setDependencyCount(1);
        render_pass_info.setPDependencies(&dependency);

        vo_.render_pass = vo_.logical_device.createRenderPass(render_pass_info);
    }

    void VulkanManager::CreateDescriptorSetLayout() {
        vk::DescriptorSetLayoutBinding descriptor_binding{};
        descriptor_binding.setBinding(0);
        descriptor_binding.setDescriptorCount(1);
        descriptor_binding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
        descriptor_binding.setPImmutableSamplers(nullptr);
        descriptor_binding.setStageFlags(vk::ShaderStageFlagBits::eVertex);

        vk::DescriptorSetLayoutBinding sampler_binding{};
        sampler_binding.setBinding(1);
        sampler_binding.setDescriptorCount(1);
        sampler_binding.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
        sampler_binding.setPImmutableSamplers(nullptr);
        sampler_binding.setStageFlags(vk::ShaderStageFlagBits::eFragment);

        std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { descriptor_binding, sampler_binding };

        vk::DescriptorSetLayoutCreateInfo descriptor_info{};
        descriptor_info.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
        descriptor_info.setBindingCount(bindings.size());
        descriptor_info.setPBindings(bindings.data());

        if (vo_.logical_device.createDescriptorSetLayout(&descriptor_info, nullptr, &vo_.descriptor_set_layout) != vk::Result::eSuccess) {
            throw std::runtime_error("Cannot create descriptor set layout.");
        }
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
        layout_info.setSetLayoutCount(1);
        layout_info.setPSetLayouts(&vo_.descriptor_set_layout);

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

    void VulkanManager::CreateTextureImage() {
        int tex_width, tex_height, tex_channels;
        stbi_uc* pixels = stbi_load(TEXTURE_IMAGE_PATH.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

        vk::DeviceSize image_size = tex_width * tex_height * 4;

        if (!pixels)
            throw std::runtime_error("Failed to load texture image.");

        vk::Buffer staging_buffer;
        vk::DeviceMemory staging_memory;

        CreateBuffer(image_size,
                     vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent),
                     staging_buffer,
                     staging_memory);
        
        void *data = vo_.logical_device.mapMemory(staging_memory, 0, image_size);
        std::memcpy(data, pixels, image_size);
        vo_.logical_device.unmapMemory(staging_memory);

        stbi_image_free(pixels);

        CreateImage(tex_width, tex_height, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                    vk::MemoryPropertyFlagBits::eDeviceLocal, vo_.texture_image, vo_.texture_memory);
    
        TransitionImageLayout(vo_.texture_image, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        CopyBufferToImage(staging_buffer, vo_.texture_image, tex_width, tex_height);
        TransitionImageLayout(vo_.texture_image, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    
        vo_.logical_device.destroyBuffer(staging_buffer);
        vo_.logical_device.freeMemory(staging_memory);
    }

    void VulkanManager::CreateTextureImageView() {
        vo_.texture_image_view = CreateImageView(vo_.texture_image, vk::Format::eR8G8B8A8Srgb);
    }

    void VulkanManager::CreateTextureSampler() {
        auto texture_settings = GraphicsSettings::SetupTextureSettings(vo_.physical_device);

        vo_.texture_sampler = vo_.logical_device.createSampler(texture_settings);
    }

    void VulkanManager::CreateVertexBuffer() {
        vk::DeviceSize buffer_size = sizeof(vo_.loader.object[0]) * vo_.loader.object.size();
        // vk::DeviceSize buffer_size = sizeof(VERTICES[0]) * VERTICES.size();

        vk::Buffer staging_buffer;
        vk::DeviceMemory staging_memory;

        CreateBuffer(buffer_size,
                     vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent),
                     staging_buffer,
                     staging_memory);

        void *data = vo_.logical_device.mapMemory(staging_memory, 0, buffer_size);
        std::memcpy(data, vo_.loader.object.data(), buffer_size);
        vo_.logical_device.unmapMemory(staging_memory);

        CreateBuffer(buffer_size,
                     vk::BufferUsageFlags(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer),
                     vk::MemoryPropertyFlags(vk::MemoryPropertyFlagBits::eDeviceLocal),
                     vo_.vertex_buffer,
                     vo_.vertex_memory);

        CopyBuffer(staging_buffer, vo_.vertex_buffer, buffer_size);

        vo_.logical_device.destroyBuffer(staging_buffer);
        vo_.logical_device.freeMemory(staging_memory);
    }

    void VulkanManager::CreateIndexBuffer() {
        vk::DeviceSize buffer_size = sizeof(INDICES[0]) * INDICES.size();

        vk::Buffer staging_buffer;
        vk::DeviceMemory staging_memory;

        CreateBuffer(buffer_size,
                     vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent),
                     staging_buffer,
                     staging_memory);

        void *data = vo_.logical_device.mapMemory(staging_memory, 0, buffer_size);
        std::memcpy(data, INDICES.data(), buffer_size);
        vo_.logical_device.unmapMemory(staging_memory);

        CreateBuffer(buffer_size,
                     vk::BufferUsageFlags(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer),
                     vk::MemoryPropertyFlags(vk::MemoryPropertyFlagBits::eDeviceLocal),
                     vo_.indices_buffer,
                     vo_.indices_memory);

        CopyBuffer(staging_buffer, vo_.indices_buffer, buffer_size);

        vo_.logical_device.destroyBuffer(staging_buffer);
        vo_.logical_device.freeMemory(staging_memory);
    }

    void VulkanManager::CreateUniformBuffers() {
        vk::DeviceSize buffer_size = sizeof(MVP);

        vo_.uniform_buffers.resize(MAX_FRAMES);
        vo_.uniform_memories.resize(MAX_FRAMES);
        vo_.uniform_maps.resize(MAX_FRAMES);
        
        for (size_t i = 0; i < MAX_FRAMES; ++i) {
            CreateBuffer(buffer_size,
                         vk::BufferUsageFlagBits::eUniformBuffer,
                         vk::MemoryPropertyFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent),
                         vo_.uniform_buffers[i],
                         vo_.uniform_memories[i]);

            vo_.uniform_maps[i] = vo_.logical_device.mapMemory(vo_.uniform_memories[i], 0, buffer_size);
        }

    }

    void VulkanManager::CreateDescriptorPool() {
        vk::DescriptorPoolSize mvp_desc_pool_size{};
        mvp_desc_pool_size.setType(vk::DescriptorType::eUniformBuffer);
        mvp_desc_pool_size.setDescriptorCount(MAX_FRAMES);

        vk::DescriptorPoolSize sampler_desc_pool_size{};
        sampler_desc_pool_size.setType(vk::DescriptorType::eCombinedImageSampler);
        sampler_desc_pool_size.setDescriptorCount(MAX_FRAMES);

        std::array<vk::DescriptorPoolSize, 2> desc_pool_sizes = { mvp_desc_pool_size, sampler_desc_pool_size };

        vk::DescriptorPoolCreateInfo desc_pool_info{};
        desc_pool_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;
        desc_pool_info.setPoolSizeCount(desc_pool_sizes.size());
        desc_pool_info.setPPoolSizes(desc_pool_sizes.data());
        desc_pool_info.setMaxSets(MAX_FRAMES);

        vo_.descriptor_pool = vo_.logical_device.createDescriptorPool(desc_pool_info);
    }

    void VulkanManager::CreateDescriptorSets() {
        std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES, vo_.descriptor_set_layout);

        vk::DescriptorSetAllocateInfo desc_set_alloc_info{};
        desc_set_alloc_info.sType = vk::StructureType::eDescriptorSetAllocateInfo;
        desc_set_alloc_info.setDescriptorPool(vo_.descriptor_pool);
        desc_set_alloc_info.setDescriptorSetCount(MAX_FRAMES);
        desc_set_alloc_info.setPSetLayouts(layouts.data());

        vo_.descriptor_sets.resize(MAX_FRAMES);
        if (vo_.logical_device.allocateDescriptorSets(&desc_set_alloc_info, vo_.descriptor_sets.data()) != vk::Result::eSuccess)
            throw std::runtime_error("Failed to create descriptor sets.");
        
        for (size_t i = 0; i < MAX_FRAMES; ++i) {
            vk::DescriptorBufferInfo desc_buffer_info{};
            desc_buffer_info.setBuffer(vo_.uniform_buffers[i]);
            desc_buffer_info.setOffset(0);
            desc_buffer_info.setRange(sizeof(MVP));

            vk::DescriptorImageInfo desc_image_info{};
            desc_image_info.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
            desc_image_info.setImageView(vo_.texture_image_view);
            desc_image_info.setSampler(vo_.texture_sampler);

            vk::WriteDescriptorSet write_uniform_desc_set{};
            write_uniform_desc_set.sType = vk::StructureType::eWriteDescriptorSet;
            write_uniform_desc_set.setDstSet(vo_.descriptor_sets[i]);
            write_uniform_desc_set.setDstBinding(0);
            write_uniform_desc_set.setDstArrayElement(0);
            write_uniform_desc_set.setDescriptorType(vk::DescriptorType::eUniformBuffer);
            write_uniform_desc_set.setDescriptorCount(1);
            write_uniform_desc_set.setPBufferInfo(&desc_buffer_info);
            write_uniform_desc_set.setPImageInfo(nullptr);
            write_uniform_desc_set.setPTexelBufferView(nullptr);

            vk::WriteDescriptorSet write_texture_desc_set{};
            write_texture_desc_set.sType = vk::StructureType::eWriteDescriptorSet;
            write_texture_desc_set.setDstSet(vo_.descriptor_sets[i]);
            write_texture_desc_set.setDstBinding(1);
            write_texture_desc_set.setDstArrayElement(0);
            write_texture_desc_set.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
            write_texture_desc_set.setDescriptorCount(1);
            write_texture_desc_set.setPImageInfo(&desc_image_info);
            write_texture_desc_set.setPTexelBufferView(nullptr);

            std::array<vk::WriteDescriptorSet, 2> write_desc_sets = { write_uniform_desc_set, write_texture_desc_set };

            vo_.logical_device.updateDescriptorSets(write_desc_sets.size(), write_desc_sets.data(), 0, nullptr);
        }
    }

    void VulkanManager::CreateCommandBuffers() {
        vk::CommandBufferAllocateInfo cmd_buff_ainfo{};
        cmd_buff_ainfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
        cmd_buff_ainfo.setCommandPool(vo_.command_pool);
        cmd_buff_ainfo.setLevel(vk::CommandBufferLevel::ePrimary);

        vo_.command_buffers.resize(MAX_FRAMES);
        cmd_buff_ainfo.setCommandBufferCount(vo_.command_buffers.size());
        if (vo_.logical_device.allocateCommandBuffers(&cmd_buff_ainfo, vo_.command_buffers.data()) != vk::Result::eSuccess)
            throw std::runtime_error("Failed to allocate command buffer.");
    }

    void VulkanManager::CreateSyncObjects() {
        vk::SemaphoreCreateInfo sem_info{};
        sem_info.sType = vk::StructureType::eSemaphoreCreateInfo;

        vk::FenceCreateInfo fence_info{};
        fence_info.sType = vk::StructureType::eFenceCreateInfo;
        fence_info.setFlags(vk::FenceCreateFlagBits::eSignaled);

        vo_.image_available_sems.resize(MAX_FRAMES);
        vo_.render_finished_sems.resize(MAX_FRAMES);
        vo_.in_flight_fences.resize(MAX_FRAMES);

        for (size_t i = 0; i < MAX_FRAMES; i++) {
            vo_.image_available_sems[i] = vo_.logical_device.createSemaphore(sem_info);
            vo_.render_finished_sems[i] = vo_.logical_device.createSemaphore(sem_info);
            vo_.in_flight_fences[i] = vo_.logical_device.createFence(fence_info);
        }
    }

    void VulkanManager::CreateObject() {
        vo_.loader.LoadObject();
    }

    void VulkanManager::DestroyEverything() {
        DestroySwapchainImages();
        vo_.logical_device.destroySwapchainKHR(vo_.swapchain);

        vo_.logical_device.destroySampler(vo_.texture_sampler);
        vo_.logical_device.destroyImageView(vo_.texture_image_view);
        vo_.logical_device.destroyImage(vo_.texture_image);
        vo_.logical_device.freeMemory(vo_.texture_memory);

        for (size_t i = 0; i < MAX_FRAMES; ++i) {
            vo_.logical_device.destroyBuffer(vo_.uniform_buffers[i]);
            vo_.logical_device.freeMemory(vo_.uniform_memories[i]);
        }

        vo_.logical_device.destroyDescriptorPool(vo_.descriptor_pool);
        vo_.logical_device.destroyDescriptorSetLayout(vo_.descriptor_set_layout);

        for (size_t i = 0; i < MAX_FRAMES; ++i) {
            vo_.logical_device.destroySemaphore(vo_.image_available_sems[i]);
            vo_.logical_device.destroySemaphore(vo_.render_finished_sems[i]);
            vo_.logical_device.destroyFence(vo_.in_flight_fences[i]);
        }

        vo_.logical_device.destroyBuffer(vo_.vertex_buffer);
        vo_.logical_device.freeMemory(vo_.vertex_memory);

        vo_.logical_device.destroyBuffer(vo_.indices_buffer);
        vo_.logical_device.freeMemory(vo_.indices_memory);

        vo_.logical_device.destroyCommandPool(vo_.command_pool);
        vo_.logical_device.destroyPipeline(vo_.pipeline);
        vo_.logical_device.destroyPipelineLayout(vo_.layout);
        vo_.logical_device.destroyRenderPass(vo_.render_pass);

        if (ENABLE_VALIDATION_LAYERS)
            vo_.instance.destroyDebugUtilsMessengerEXT(vo_.debug_messenger, nullptr, vk::DispatchLoaderDynamic(vo_.instance, vkGetInstanceProcAddr));

        vo_.logical_device.destroy();
        vo_.instance.destroySurfaceKHR(vo_.surface);
        vo_.instance.destroy();
    }

    void VulkanManager::DestroySwapchainImages() {
        for (auto framebuffer : vo_.framebuffers)
            vo_.logical_device.destroyFramebuffer(framebuffer);

        for (auto image_view : vo_.image_views)
            vo_.logical_device.destroyImageView(image_view);
    }

    vk::ImageView VulkanManager::CreateImageView(vk::Image image, vk::Format format) {
        vk::ImageViewCreateInfo image_info{};
        image_info.sType = vk::StructureType::eImageViewCreateInfo;
        image_info.setImage(image);
        image_info.setViewType(vk::ImageViewType::e2D);
        image_info.setFormat(format);
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

        return vo_.logical_device.createImageView(image_info);
    }

    void VulkanManager::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &memory)
    {
        vk::BufferCreateInfo buffer_info{};
        buffer_info.sType = vk::StructureType::eBufferCreateInfo;
        buffer_info.setSize(size);
        buffer_info.setUsage(usage);
        buffer_info.setSharingMode(vk::SharingMode::eExclusive);
        
        buffer = vo_.logical_device.createBuffer(buffer_info);

        vk::MemoryRequirements mem_reqs = vo_.logical_device.getBufferMemoryRequirements(buffer);
        
        vk::MemoryAllocateInfo alloc_info{};
        alloc_info.sType = vk::StructureType::eMemoryAllocateInfo;
        alloc_info.setAllocationSize(mem_reqs.size);
        alloc_info.setMemoryTypeIndex(
            vo_.validator.ChooseDeviceMemoryType(
                mem_reqs.memoryTypeBits,
                properties,
                vo_.physical_device
            )
        );

        memory = vo_.logical_device.allocateMemory(alloc_info);
        vo_.logical_device.bindBufferMemory(buffer, memory, 0);
    }

    void VulkanManager::CopyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size) {
        vk::CommandBuffer cmd_buffer = BeginSingletimeCommand();

        vk::BufferCopy buff_copy{};
        buff_copy.setDstOffset(0);
        buff_copy.setSrcOffset(0);
        buff_copy.setSize(size);

        cmd_buffer.copyBuffer(src, dst, 1, &buff_copy);

        EndSingletimeCommand(cmd_buffer);
    }

    void VulkanManager::CreateImage(uint32_t width,
                                    uint32_t height,
                                    vk::Format format,
                                    vk::ImageTiling tiling,
                                    vk::ImageUsageFlags usage,
                                    vk::MemoryPropertyFlags properties,
                                    vk::Image &image,
                                    vk::DeviceMemory &memory) {
        vk::ImageCreateInfo image_info{};
        image_info.sType = vk::StructureType::eImageCreateInfo;
        image_info.setImageType(vk::ImageType::e2D);
        image_info.setExtent(vk::Extent3D(width, height, 1));
        image_info.setMipLevels(1);
        image_info.setArrayLayers(1);
        image_info.setFormat(format);
        image_info.setTiling(tiling);
        image_info.setInitialLayout(vk::ImageLayout::eUndefined);
        image_info.setUsage(usage);
        image_info.setSharingMode(vk::SharingMode::eExclusive);
        image_info.setSamples(vk::SampleCountFlagBits::e1);

        image = vo_.logical_device.createImage(image_info);

        vk::MemoryRequirements mem_reqs = vo_.logical_device.getImageMemoryRequirements(image);

        vk::MemoryAllocateInfo mem_alloc_info{};
        mem_alloc_info.sType = vk::StructureType::eMemoryAllocateInfo;
        mem_alloc_info.setAllocationSize(mem_reqs.size);
        mem_alloc_info.setMemoryTypeIndex(vo_.validator.ChooseDeviceMemoryType(mem_reqs.memoryTypeBits, properties, vo_.physical_device));

        memory = vo_.logical_device.allocateMemory(mem_alloc_info);
        vo_.logical_device.bindImageMemory(image, memory, 0);
    }

    void VulkanManager::TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout old_layout, vk::ImageLayout new_layout) {
        vk::CommandBuffer cmd_buffer = BeginSingletimeCommand();

        vk::ImageMemoryBarrier memory_barrier{};
        memory_barrier.sType = vk::StructureType::eImageMemoryBarrier;
        memory_barrier.setOldLayout(old_layout);
        memory_barrier.setNewLayout(new_layout);

        memory_barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
        memory_barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);

        memory_barrier.setImage(image);
        memory_barrier.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));


        vk::PipelineStageFlags source_stage;
        vk::PipelineStageFlags dest_stage;

        if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eTransferDstOptimal) {
            memory_barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
            memory_barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

            source_stage = vk::PipelineStageFlagBits::eTopOfPipe;
            dest_stage = vk::PipelineStageFlagBits::eTransfer;
        } else if (old_layout == vk::ImageLayout::eTransferDstOptimal && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            memory_barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
            memory_barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

            source_stage = vk::PipelineStageFlagBits::eTransfer;
            dest_stage = vk::PipelineStageFlagBits::eFragmentShader;
        }

        cmd_buffer.pipelineBarrier(
            source_stage, dest_stage,
            vk::DependencyFlags(),
            0, nullptr,
            0, nullptr,
            1, &memory_barrier
        );

        EndSingletimeCommand(cmd_buffer);
    }

    void VulkanManager::CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height) {
        vk::CommandBuffer cmd_buffer = BeginSingletimeCommand();

        vk::BufferImageCopy image_copy{};
        image_copy.setBufferOffset(0);
        image_copy.setBufferRowLength(0);
        image_copy.setBufferImageHeight(0);
        image_copy.setImageSubresource({vk::ImageAspectFlagBits::eColor, 0, 0, 1});
        image_copy.setImageOffset({0, 0, 0});
        image_copy.setImageExtent({width, height, 1});

        cmd_buffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &image_copy);

        EndSingletimeCommand(cmd_buffer);
    }

    vk::CommandBuffer VulkanManager::BeginSingletimeCommand() {
        vk::CommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
        alloc_info.setLevel(vk::CommandBufferLevel::ePrimary);
        alloc_info.setCommandPool(vo_.command_pool);
        alloc_info.setCommandBufferCount(1);

        vk::CommandBuffer cmd_buffer;
        if (vo_.logical_device.allocateCommandBuffers(&alloc_info, &cmd_buffer) != vk::Result::eSuccess)
            throw std::runtime_error("Cannot allocate singletime command buffer.");

        vk::CommandBufferBeginInfo begin_info{};
        begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
        begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmd_buffer.begin(begin_info);

        return cmd_buffer;
    }

    void VulkanManager::EndSingletimeCommand(vk::CommandBuffer cmd_buffer) {
        cmd_buffer.end();

        vk::SubmitInfo submit{};
        submit.sType = vk::StructureType::eSubmitInfo;
        submit.setCommandBufferCount(1);
        submit.setPCommandBuffers(&cmd_buffer);

        if (vo_.graphics_queue.submit(1, &submit, VK_NULL_HANDLE) != vk::Result::eSuccess)
            throw std::runtime_error("Cannot submit singletime command to graphic queue.");

        vo_.graphics_queue.waitIdle();

        vo_.logical_device.freeCommandBuffers(vo_.command_pool, 1, &cmd_buffer);
    }

    void VulkanManager::FillDebugInfo(vk::DebugUtilsMessengerCreateInfoEXT &debug_info)
    {
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

    vk::Device& VulkanManager::get_logical_device() {
        return vo_.logical_device;
    }
}
