#include "queue_families.h"

namespace mvk {
    bool QueueFamilyIndices::IsComplete() {
        return graphics_family_.has_value() && present_family_.has_value();
    }

    QueueFamilyIndices QueueFamilyIndices::FindQueueFamily(vk::PhysicalDevice& device, vk::SurfaceKHR& surface) {
        QueueFamilyIndices indices;
        auto queue_families = device.getQueueFamilyProperties();

        uint32_t i = 0;
        for (auto &qfamily : queue_families) {
            if (qfamily.queueFlags & vk::QueueFlagBits::eGraphics)
                indices.graphics_family_ = i;
            
            if (device.getSurfaceSupportKHR(i, surface))
                indices.present_family_ = i;

            i++;

            if (indices.IsComplete())
                break;
        }

        return indices;
    }
}
