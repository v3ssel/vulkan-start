#include "DisplayWindow.h"

static void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<mvk::DisplayWindow*>(glfwGetWindowUserPointer(window));
    app->SetResizeTrigger();
}

namespace mvk {
    void DisplayWindow::Run() {
        InitWindow();
        InitVulkan();
        MainLoop();
        CleanUp();      
    }

    void DisplayWindow::SetResizeTrigger() {
        return screen.set_window_resize();
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
        screen.Setup(window_);
        // screen.PrintLoadedData();
    }

    void DisplayWindow::MainLoop() {
        while(!glfwWindowShouldClose(window_)) {
            glfwPollEvents();
            screen.DrawFrame();
        }
        screen.get_logical_device().waitIdle();
    }

    void DisplayWindow::CleanUp() {
        screen.DestroyEverything();
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
}
