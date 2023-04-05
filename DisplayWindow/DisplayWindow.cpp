#include "DisplayWindow.h"

static void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<mvk::DisplayWindow*>(glfwGetWindowUserPointer(window));
    app->get_manager().set_window_resize();
}

namespace mvk {
    void DisplayWindow::Run() {
        InitWindow();
        InitVulkan();
        MainLoop();
        CleanUp();      
    }

    mvk::VulkanManager& DisplayWindow::get_manager() {
        return VulkanWrapped;
    }

    void DisplayWindow::InitWindow() {
        if (glfwInit() == GLFW_FALSE)
            throw std::runtime_error("Cannot initialize GLFW.");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        
        window_ = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan window", nullptr, nullptr);
        if (window_ == GLFW_FALSE)
            throw std::runtime_error("Cannot create window.");
        
        glfwSetWindowUserPointer(window_, this);
        glfwSetFramebufferSizeCallback(window_, FramebufferResizeCallback);
    }

    void DisplayWindow::InitVulkan() {
        VulkanWrapped.CreateInstance();
        VulkanWrapped.SetupDebug();
        VulkanWrapped.CreateSurface(window_);
        VulkanWrapped.TakeVideocard();
        VulkanWrapped.CreateLogicalDevice();
        VulkanWrapped.CreateSwapChain();
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
