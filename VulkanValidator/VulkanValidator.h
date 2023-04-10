#ifndef VULKAN_VALIDATOR
#define VULKAN_VALIDATOR

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "../QueueFamilies/QueueFamilies.h"
#include "../SwapChainDetails/SwapChainDetails.h"
#include <iostream>

namespace mvk {
    class VulkanValidator {
       public:
        void CheckRequestedInstanceExtensions(std::vector<const char*> requiement_extensions);
        std::vector<const char*> SetRequirmentInstanceExtension(bool enable_validation_layers, std::vector<const char*> instance_extensions);
        bool CheckValidationLayersSupport(std::vector<const char *> validation_layers);
        bool CheckVideocard(vk::PhysicalDevice device, vk::SurfaceKHR surface, std::vector<const char *> device_required_ext);
        bool CheckDeviceExtensions(vk::PhysicalDevice device, std::vector<const char *> device_required_ext);
    };
}

#endif // VULKAN_VALIDATOR
