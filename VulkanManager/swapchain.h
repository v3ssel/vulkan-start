#ifndef SWAPCHAIN
#define SWAPCHAIN

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace mvk {
    class SwapChain {
       public:
        SwapChain(vk::PhysicalDevice device, vk::SurfaceKHR surface);

        vk::SurfaceCapabilitiesKHR capabilities_;
        std::vector<vk::SurfaceFormatKHR> format_;
        std::vector<vk::PresentModeKHR> present_modes_;
};
}

#endif // SWAPCHAIN