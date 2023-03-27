#ifndef VULKAN_MANAGER
#define VULKAN_MANAGER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>

namespace mvk {
    #ifdef NDEBUG
        constexpr bool enableValidationLayers = false;
    #else
        constexpr bool enableValidationLayers = true;
    #endif

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation",
    };

    class VulkanManager {
        public:
         void CreateInstance();
         void SetupDebug();
         void TakeVideocard();
         void CreateLogicalDevice();

        
         vk::Instance instance_;
         vk::DebugUtilsMessengerEXT debug_messenger_;
         vk::PhysicalDevice physical_device_ = VK_NULL_HANDLE;
         vk::Device logical_device_;
         vk::Queue graphics_queue_;
        private:
         void PrintLoadedData();
         void CheckRequestedExtensions(std::vector<const char*> requiement_extensions);
         bool CheckValidationLayersSupport();
         bool CheckVideocard(vk::PhysicalDevice device);

         void FillDebugInfo(vk::DebugUtilsMessengerCreateInfoEXT &debug_info);


         std::vector<const char*> GetRequirmentExtension();

    };
}

#endif  // VULKAN_MANAGER
