#ifndef MVK_VALIDATOR
#define MVK_VALIDATOR

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <iostream>

#include "../QueueFamilies/QueueFamilies.h"
#include "../SwapChainDetails/SwapChainDetails.h"

namespace mvk {
    class VulkanValidator {
       public:
        std::vector<const char*> SetRequirmentInstanceExtension(bool enable_validation_layers, std::vector<const char*> instance_extensions);
        
        void CheckRequestedInstanceExtensions(std::vector<const char*> requiement_extensions);
        bool CheckValidationLayersSupport(std::vector<const char *> validation_layers);
        bool CheckVideocard(vk::PhysicalDevice device, vk::SurfaceKHR surface, std::vector<const char *> device_required_ext);
        bool CheckDeviceExtensions(vk::PhysicalDevice device, std::vector<const char *> device_required_ext);
        uint32_t ChooseDeviceMemoryType(uint32_t filter, vk::MemoryPropertyFlags mem_properties, vk::PhysicalDevice& physical_device);
    };
}

#endif  // MVK_VALIDATOR
