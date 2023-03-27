#include "../DisplayWindow/display_window.h"

namespace mvk {
    void Triangle::Run() {
        InitWindow();
        InitVulkan();
        MainLoop();
        CleanUp();      
    }

    void Triangle::InitWindow() {
        if (glfwInit() == GLFW_FALSE) throw std::runtime_error("Cannot initialize GLFW.");

        // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window_ = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan window", nullptr, nullptr);
        if (window_ == GLFW_FALSE) throw std::runtime_error("Cannot create window.");

        // glfwMakeContextCurrent(window_);
    }

    void Triangle::InitVulkan() {
        VulkanWrapped.CreateInstance();
        VulkanWrapped.SetupDebug();
        VulkanWrapped.TakeVideocard();
        VulkanWrapped.CreateLogicalDevice();
    }

    void Triangle::MainLoop() {
        while(!glfwWindowShouldClose(window_)) {
            glClear(GL_COLOR_BUFFER_BIT);

            srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            glClearColor(((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)));

            glfwSwapBuffers(window_);
            glfwPollEvents();
        }        
    }

    void Triangle::CleanUp() {
        if (enableValidationLayers)
            VulkanWrapped.instance_.destroyDebugUtilsMessengerEXT(VulkanWrapped.debug_messenger_, nullptr, vk::DispatchLoaderDynamic(VulkanWrapped.instance_, vkGetInstanceProcAddr));
        
        VulkanWrapped.logical_device_.destroy();
        VulkanWrapped.instance_.destroy();
        glfwDestroyWindow(window_);

        glfwTerminate();
    }
}
