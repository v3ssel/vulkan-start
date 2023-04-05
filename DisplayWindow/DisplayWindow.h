#ifndef DISPLAY_WINDOW
#define DISPLAY_WINDOW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <chrono>

#include "../VulkanManager/VulkanManager.h"

constexpr int WIDTH = 1280;
constexpr int HEIGHT = 720;

namespace mvk {
    class DisplayWindow {
       public:
        void Run();
        mvk::VulkanManager& get_manager();

       private:
        void InitWindow();
        void InitVulkan();
        void MainLoop();
        void CleanUp();

        void CreateSurface();

        GLFWwindow* window_;
        mvk::VulkanManager VulkanWrapped; 
    };
}

#endif  // DISPLAY_WINDOW
