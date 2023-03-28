#ifndef QUEUE_FAMILIES
#define QUEUE_FAMILIES

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <optional>

namespace mvk {
    class QueueFamilies {
       public:
        std::optional<uint32_t> graphics_family_;
        std::optional<uint32_t> present_family_;

        bool IsComplete();
        static QueueFamilies FindQueueFamily(vk::PhysicalDevice& device, vk::SurfaceKHR& surface);
};
}

#endif // QUEUE_FAMILIES
