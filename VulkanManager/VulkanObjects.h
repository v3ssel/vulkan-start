#ifndef VULKAN_OBJECTS
#define VULKAN_OBJECTS

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "../VulkanValidator/VulkanValidator.h"
#include "../Shaders/ShadersHelper.h"

namespace mvk {
    struct VulkanObjects {
        vk::Instance instance;
        vk::DebugUtilsMessengerEXT debug_messenger;

        vk::PhysicalDevice physical_device = VK_NULL_HANDLE;
        vk::Device logical_device;
        
        vk::Queue graphics_queue;
        vk::Queue present_queue;
        
        vk::SurfaceKHR surface;
        vk::SwapchainKHR swapchain;
        std::vector<vk::Image> swapchain_images;
        vk::Extent2D sc_extent;
        vk::Format sc_format;

        std::vector<vk::ImageView> image_views;

        vk::PipelineLayout layout;

        VulkanValidator validator;
    };
}

#endif // VULKAN_OBJECTS
