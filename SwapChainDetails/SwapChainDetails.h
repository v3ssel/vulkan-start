#ifndef SWAPCHAIN
#define SWAPCHAIN

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <cstdint>
#include <limits>

namespace mvk {
    class SwapChainDetails {
       public:
        SwapChainDetails(vk::PhysicalDevice device, vk::SurfaceKHR surface);

        vk::SurfaceFormatKHR ChooseSwapSurfaceFormat();
        vk::PresentModeKHR ChooseSwapPresentMode();
        vk::Extent2D ChooseSwapExtent(GLFWwindow* window);

        vk::SurfaceCapabilitiesKHR capabilities_;
        std::vector<vk::SurfaceFormatKHR> format_;
        std::vector<vk::PresentModeKHR> present_modes_;
};
}

#endif // SWAPCHAIN
