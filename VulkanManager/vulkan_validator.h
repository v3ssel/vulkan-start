#ifndef VULKAN_VALIDATOR
#define VULKAN_VALIDATOR

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "queue_families.h"
#include "swapchain.h"
#include <iostream>

namespace mvk {
    class VulkanValidator {
       public:
        void CheckRequestedExtensions(std::vector<const char*> requiement_extensions);
        std::vector<const char*> GetRequirmentInstanceExtension(bool enable_validation_layers);
        bool CheckValidationLayersSupport(std::vector<const char *> validation_layers);
        bool CheckVideocard(vk::PhysicalDevice device, vk::SurfaceKHR surface, std::vector<const char *> device_required_ext);
        bool CheckDeviceExtensions(vk::PhysicalDevice device, std::vector<const char *> device_required_ext);
    };
}

#endif // VULKAN_VALIDATOR
