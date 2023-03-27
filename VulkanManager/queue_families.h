#ifndef QUEUE_FAMILIES
#define QUEUE_FAMILIES

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <optional>

class QueueFamilyIndices {
   public:
    std::optional<uint32_t> family_;

    bool IsComplete() {
        return family_.has_value();
    }

    static QueueFamilyIndices FindQueueFamily(vk::PhysicalDevice device) {
        QueueFamilyIndices indices;
        auto queue_families = device.getQueueFamilyProperties();

        uint32_t i = 0;
        for (auto &qfamily : queue_families) {
            if (qfamily.queueFlags & vk::QueueFlagBits::eGraphics)
                indices.family_ = i;

            i++;

            if (indices.IsComplete())
                break;
        }

        return indices;
    }
};

#endif // QUEUE_FAMILIES
