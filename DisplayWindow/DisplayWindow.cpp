#include "DisplayWindow.h"

namespace mvk {
    void Triangle::Run() {
        InitWindow();
        InitVulkan();
        MainLoop();
        CleanUp();      
    }

    void Triangle::InitWindow() {
        if (glfwInit() == GLFW_FALSE) throw std::runtime_error("Cannot initialize GLFW.");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window_ = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan window", nullptr, nullptr);
        if (window_ == GLFW_FALSE) throw std::runtime_error("Cannot create window.");

        // glfwMakeContextCurrent(window_);
    }

    void Triangle::InitVulkan() {
        VulkanWrapped.CreateInstance();
        VulkanWrapped.SetupDebug();
        CreateSurface();
        VulkanWrapped.TakeVideocard();
        VulkanWrapped.CreateLogicalDevice();
        VulkanWrapped.CreateSwapChain(window_);
        VulkanWrapped.CreateImageView();
        VulkanWrapped.CreateGraphicsPipeline();

        // VulkanWrapped.PrintLoadedData();
    }

    void Triangle::MainLoop() {
        while(!glfwWindowShouldClose(window_)) {
            // glClear(GL_COLOR_BUFFER_BIT);

            // srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            // glClearColor(((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)));

            // glfwSwapBuffers(window_);
            glfwPollEvents();
        }        
    }

    void Triangle::CleanUp() {
        if (ENABLE_VALIDATION_LAYERS)
            VulkanWrapped.instance_.destroyDebugUtilsMessengerEXT(VulkanWrapped.debug_messenger_, nullptr, vk::DispatchLoaderDynamic(VulkanWrapped.instance_, vkGetInstanceProcAddr));
        
        for (auto image_view : VulkanWrapped.image_views) {
            VulkanWrapped.logical_device_.destroyImageView(image_view);
        }
        VulkanWrapped.logical_device_.destroySwapchainKHR(VulkanWrapped.swapchain_);
        VulkanWrapped.logical_device_.destroy();
        VulkanWrapped.instance_.destroySurfaceKHR(VulkanWrapped.surface_);
        VulkanWrapped.instance_.destroy();
        glfwDestroyWindow(window_);

        glfwTerminate();
    }

    void Triangle::CreateSurface() {
        VkSurfaceKHR surface;

        if (glfwCreateWindowSurface(VulkanWrapped.instance_, window_, nullptr, &surface) != VK_SUCCESS)
            throw std::runtime_error("Failed to create window surface.");
        VulkanWrapped.surface_ = vk::SurfaceKHR(surface);
    }
}
