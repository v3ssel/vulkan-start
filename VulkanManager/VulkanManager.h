#ifndef VULKAN_MANAGER
#define VULKAN_MANAGER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>

#include "../VulkanValidator/VulkanValidator.h"

namespace mvk {
    #ifdef NDEBUG
        constexpr bool ENABLE_VALIDATION_LAYERS = false;
    #else
        constexpr bool ENABLE_VALIDATION_LAYERS = true;
    #endif

    const std::vector<const char*> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation",
    };

    const std::vector<const char*> DEVICE_REQUIRED_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    class VulkanManager {
        public:
         void CreateInstance();
         void SetupDebug();
         void TakeVideocard();
         void CreateLogicalDevice();
         void CreateSwapChain(GLFWwindow *window);

        
         vk::Instance instance_;
         vk::DebugUtilsMessengerEXT debug_messenger_;
         vk::SurfaceKHR surface_;
         vk::PhysicalDevice physical_device_ = VK_NULL_HANDLE;
         vk::Device logical_device_;
         vk::Queue graphics_queue_;
         vk::Queue present_queue_;
         vk::SwapchainKHR swapchain_;
         
         std::vector<vk::Image> swapchain_images_;
         vk::Extent2D sc_extent_;
         vk::Format sc_format_;

         VulkanValidator validator_;

         void PrintLoadedData();
         
        private:
         void FillDebugInfo(vk::DebugUtilsMessengerCreateInfoEXT &debug_info);
    };
}

#endif  // VULKAN_MANAGER
