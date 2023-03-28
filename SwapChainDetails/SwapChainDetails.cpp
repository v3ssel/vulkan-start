#include "SwapChainDetails.h"

namespace mvk {
    SwapChainDetails::SwapChainDetails(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
        capabilities_ = device.getSurfaceCapabilitiesKHR(surface);
        format_ = device.getSurfaceFormatsKHR(surface);
        present_modes_ = device.getSurfacePresentModesKHR(surface);
    }
    
    VkSurfaceFormatKHR SwapChainDetails::ChooseSwapSurfaceFormat() {

        return VkSurfaceFormatKHR();
    }
    
    VkSurfaceFormatKHR SwapChainDetails::ChooseSwapPresentMode() {
        return VkSurfaceFormatKHR();
    }
    
    VkSurfaceFormatKHR SwapChainDetails::ChooseSwapExtent() {
        return VkSurfaceFormatKHR();
    }
}
