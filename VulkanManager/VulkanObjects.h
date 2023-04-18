#ifndef MVK_OBJECTS
#define MVK_OBJECTS

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "../VulkanValidator/VulkanValidator.h"
#include "../ObjectLoader/ObjectLoader.h"

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
        vk::RenderPass render_pass;
        vk::Pipeline pipeline;

        std::vector<vk::Framebuffer> framebuffers;

        vk::CommandPool command_pool;
        std::vector<vk::CommandBuffer> command_buffers;

        std::vector<vk::Semaphore> image_available_sems;
        std::vector<vk::Semaphore> render_finished_sems;
        std::vector<vk::Fence> in_flight_fences;

        VulkanValidator validator;
        
        ObjectLoader loader;
        vk::Buffer vertex_buffer;
        vk::DeviceMemory vertex_buffer_memory;    
    };
}

#endif  // MVK_OBJECTS
