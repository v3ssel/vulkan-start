#include "swapchain.h"

namespace mvk {
    SwapChain::SwapChain(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
        capabilities_ = device.getSurfaceCapabilitiesKHR(surface);
        format_ = device.getSurfaceFormatsKHR(surface);
        present_modes_ = device.getSurfacePresentModesKHR(surface);
    }
}
