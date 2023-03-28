#ifndef DISPLAY_WINDOW
#define DISPLAY_WINDOW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <chrono>

#include "../VulkanManager/VulkanManager.h"

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

namespace mvk {
    class Triangle {
        public:
         void Run();
        
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
