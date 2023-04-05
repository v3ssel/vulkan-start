#include "DisplayWindow.h"

namespace mvk {
    void DisplayWindow::Run() {
        InitWindow();
        InitVulkan();
        MainLoop();
        CleanUp();      
    }

    void DisplayWindow::InitWindow() {
        if (glfwInit() == GLFW_FALSE)
            throw std::runtime_error("Cannot initialize GLFW.");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        
        window_ = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan window", nullptr, nullptr);
        if (window_ == GLFW_FALSE)
            throw std::runtime_error("Cannot create window.");
    }

    void DisplayWindow::InitVulkan() {
        VulkanWrapped.CreateInstance();
        VulkanWrapped.SetupDebug();
        VulkanWrapped.CreateSurface(window_);
        VulkanWrapped.TakeVideocard();
        VulkanWrapped.CreateLogicalDevice();
        VulkanWrapped.CreateSwapChain(window_);
        VulkanWrapped.CreateImageView();
        VulkanWrapped.CreateRenderPass();
        VulkanWrapped.CreateGraphicsPipeline();
        VulkanWrapped.CreateFramebuffers();
        VulkanWrapped.CreateCommandPool();
        VulkanWrapped.CreateCommandBuffers();
        VulkanWrapped.CreateSyncObjects();

        // VulkanWrapped.PrintLoadedData();
    }

    void DisplayWindow::MainLoop() {
        while(!glfwWindowShouldClose(window_)) {
            glfwPollEvents();
            VulkanWrapped.DrawFrame();
        }
        VulkanWrapped.get_logical_device().waitIdle();
    }

    void DisplayWindow::CleanUp() {
        VulkanWrapped.DestroyEverything();
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
}
