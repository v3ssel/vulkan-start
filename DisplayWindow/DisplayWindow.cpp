#include "DisplayWindow.h"

namespace mvk {
    void DisplayWindow::Run() {
        InitWindow();
        InitVulkan();
        MainLoop();
        CleanUp();      
    }

    void DisplayWindow::InitWindow() {
        if (glfwInit() == GLFW_FALSE) throw std::runtime_error("Cannot initialize GLFW.");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window_ = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan window", nullptr, nullptr);
        if (window_ == GLFW_FALSE) throw std::runtime_error("Cannot create window.");

        // glfwMakeContextCurrent(window_);
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

        // VulkanWrapped.PrintLoadedData();
    }

    void DisplayWindow::MainLoop() {
        while(!glfwWindowShouldClose(window_)) {
            // glClear(GL_COLOR_BUFFER_BIT);

            // srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            // glClearColor(((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)));

            // glfwSwapBuffers(window_);
            glfwPollEvents();
        }        
    }

    void DisplayWindow::CleanUp() {
        VulkanWrapped.DestroyEverything();
        glfwDestroyWindow(window_);

        glfwTerminate();
    }
}
