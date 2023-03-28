#include "QueueFamilies.h"

namespace mvk {
    bool QueueFamilies::IsComplete() {
        return graphics_family_.has_value() && present_family_.has_value();
    }

    QueueFamilies QueueFamilies::FindQueueFamily(vk::PhysicalDevice& device, vk::SurfaceKHR& surface) {
        QueueFamilies indices;
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
