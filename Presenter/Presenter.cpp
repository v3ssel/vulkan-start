#include "Presenter.h"

void mvk::VKPresenter::Setup(GLFWwindow* window) {
    this->CreateInstance();
    this->SetupDebug();
    this->CreateSurface(window);
    this->TakeVideocard();
    this->CreateLogicalDevice();
    this->CreateSwapChain();
    this->CreateImageView();
    this->CreateRenderPass();
    this->CreateGraphicsPipeline();
    this->CreateFramebuffers();
    this->CreateCommandPool();
    this->CreateVertexBuffer();
    this->CreateCommandBuffers();
    this->CreateSyncObjects();
    this->CreateObject(VERTICES);
}

void mvk::VKPresenter::DrawFrame() {
    if (vo_.logical_device.waitForFences(1, &vo_.in_flight_fences[current_frame_], VK_TRUE, UINT64_MAX) != vk::Result::eSuccess)
        throw std::runtime_error("Cannot wait for fences.");
    
    vk::ResultValue<uint32_t> res = vo_.logical_device.acquireNextImageKHR(vo_.swapchain, UINT64_MAX, vo_.image_available_sems[current_frame_]);
    
    if (res.result == vk::Result::eErrorOutOfDateKHR) {
        window_resized_ = false;
        RecreateSwapChain();
        return;
    } else if (res.result != vk::Result::eSuccess && res.result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Cannot acquire next image.");
    }

    if (vo_.logical_device.resetFences(1, &vo_.in_flight_fences[current_frame_]) != vk::Result::eSuccess)
        throw std::runtime_error("Cannot reset fences.");

    vo_.command_buffers[current_frame_].reset();
    RecordCommandBuffer(vo_.command_buffers[current_frame_], res.value);


    vk::SubmitInfo submit_info{};
    submit_info.sType = vk::StructureType::eSubmitInfo;
    vk::Semaphore waiting_sems[] = { vo_.image_available_sems[current_frame_] };
    submit_info.setWaitSemaphoreCount(1);
    submit_info.setPWaitSemaphores(waiting_sems);
    vk::PipelineStageFlags waiting_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submit_info.setPWaitDstStageMask(waiting_stages);
    submit_info.setCommandBufferCount(1);
    submit_info.setPCommandBuffers(&vo_.command_buffers[current_frame_]);
    vk::Semaphore signal_sems[] = { vo_.render_finished_sems[current_frame_] };
    submit_info.setSignalSemaphoreCount(1);
    submit_info.setPSignalSemaphores(signal_sems);

    if (vo_.graphics_queue.submit(1, &submit_info, vo_.in_flight_fences[current_frame_]) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to submit drawing.");
    
    
    vk::PresentInfoKHR present{};
    present.sType = vk::StructureType::ePresentInfoKHR;
    present.setWaitSemaphoreCount(1);
    present.setPWaitSemaphores(signal_sems);
    vk::SwapchainKHR swap_chains[] = { vo_.swapchain };
    present.setSwapchainCount(1);
    present.setPSwapchains(swap_chains);
    present.setPImageIndices(&res.value);
    present.setPResults(nullptr);

    vk::Result present_res = vo_.present_queue.presentKHR(&present);
    if (present_res == vk::Result::eErrorOutOfDateKHR || present_res == vk::Result::eSuboptimalKHR || window_resized_) {
        window_resized_ = false;
        RecreateSwapChain();
    } else if (res.result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present image.");
    }
    
    current_frame_ = (current_frame_ + 1) % MAX_FRAMES;
}

void mvk::VKPresenter::RecordCommandBuffer(vk::CommandBuffer command_buffer, uint32_t image_index) {
    vk::CommandBufferBeginInfo begin_info{};
    begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
    begin_info.setPInheritanceInfo(nullptr);

    if (command_buffer.begin(&begin_info) != vk::Result::eSuccess) {
        std::runtime_error("Failed to begin recording command buffer.");
    }
    

    vk::RenderPassBeginInfo render_pass_begin_info{};
    render_pass_begin_info.sType = vk::StructureType::eRenderPassBeginInfo;
    render_pass_begin_info.setRenderPass(this->vo_.render_pass);
    render_pass_begin_info.setFramebuffer(this->vo_.framebuffers[image_index]);
    render_pass_begin_info.setRenderArea(vk::Rect2D({0, 0}, this->vo_.sc_extent));
    
    vk::ClearValue clear_color(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f));
    
    render_pass_begin_info.setPClearValues(&clear_color);
    render_pass_begin_info.setClearValueCount(1);
    command_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
    command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, this->vo_.pipeline);
    
    command_buffer.setPolygonModeEXT(vk::PolygonMode::eFill, vk::DispatchLoaderDynamic(this->vo_.instance, vkGetInstanceProcAddr));

    vk::Viewport viewport{};
    viewport.setX(0.0f);
    viewport.setY(0.0f);
    viewport.setWidth(static_cast<float>(this->vo_.sc_extent.width));
    viewport.setHeight(static_cast<float>(this->vo_.sc_extent.height));
    viewport.setMinDepth(0.0f);
    viewport.setMaxDepth(1.0f);
    command_buffer.setViewport(0, 1, &viewport);

    vk::Rect2D scissor{};
    scissor.setOffset(vk::Offset2D((double)0, (double)0));
    scissor.setExtent(this->vo_.sc_extent);
    command_buffer.setScissor(0, 1, &scissor);

    vk::Buffer vertex_buff[] = {vo_.vertex_buffer};
    vk::DeviceSize offsets[] = {0};
    command_buffer.bindVertexBuffers(0, 1, vertex_buff, offsets); 

    command_buffer.draw(static_cast<uint32_t>(VERTICES.size()), 1, 0, 0);

    // command_buffer.setPolygonModeEXT(vk::PolygonMode::eLine, vk::DispatchLoaderDynamic(vo->instance, vkGetInstanceProcAddr));
    // command_buffer.draw(3, 1, 0, 0);

    command_buffer.endRenderPass();

    command_buffer.end();
}

void mvk::VKPresenter::PrintLoadedData() {
    auto extensions = vk::enumerateInstanceExtensionProperties();
    std::cout << "\u001b[36mINSTANCE EXTENSIONS:\n";
    for (auto &ext : extensions)
        std::cout << '\t' << ext.extensionName << '\n';


    auto layers = vk::enumerateInstanceLayerProperties();
    std::cout << "LAYERS:\n";
    for (auto &layer : layers)
        std::cout << '\t' << layer.layerName << "\n";

    auto devices = this->vo_.instance.enumeratePhysicalDevices();
    std::cout << "DEVICES:\n";
    for (auto &device : devices) {
        std::cout << "\t" << device.getProperties().deviceName << "\n";
        
        // std::cout << "\tEXTENSIONS:\n";
        // for (auto& device_ext : device.enumerateDeviceExtensionProperties())
        //     std::cout << "\t\t" << device_ext.extensionName << "\n";

        std::cout << "\t" << "LAYERS:\n";
        for (auto& layer : device.enumerateDeviceLayerProperties())
            std::cout << "\t\t" << layer.layerName << "\n";

        std::cout << "\t" << "QUEUES:\n";
        for (auto &queue : device.getQueueFamilyProperties())
            std::cout << "\t\t" << queue.queueCount << "\n";
    }
    std::cout << "\u001b[0m";
}

void mvk::VKPresenter::set_window_resize() {
    window_resized_ = true;
}
