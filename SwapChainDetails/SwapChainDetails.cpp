#include "SwapChainDetails.h"

namespace mvk {
    SwapChainDetails::SwapChainDetails(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
        capabilities_ = device.getSurfaceCapabilitiesKHR(surface);
        format_ = device.getSurfaceFormatsKHR(surface);
        present_modes_ = device.getSurfacePresentModesKHR(surface);
    }
    
    vk::SurfaceFormatKHR SwapChainDetails::ChooseSwapSurfaceFormat() {
        for (auto &format : this->format_)
            if (format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear &&
                format.format == vk::Format::eB8G8R8A8Srgb)
                return format;

        return this->format_[0];
    }
    
    vk::PresentModeKHR SwapChainDetails::ChooseSwapPresentMode() {
        for (auto &mode : this->present_modes_)
            if (mode == vk::PresentModeKHR::eMailbox)
                return mode;

        return vk::PresentModeKHR::eFifo;
    }
    
    vk::Extent2D SwapChainDetails::ChooseSwapExtent(GLFWwindow* window) {
        if (this->capabilities_.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return this->capabilities_.currentExtent;
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        vk::Extent2D actual(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

        actual.width = std::clamp(actual.width,
                                  this->capabilities_.minImageExtent.width,
                                  this->capabilities_.maxImageExtent.width);

        actual.height = std::clamp(actual.height,
                                  this->capabilities_.minImageExtent.height,
                                  this->capabilities_.maxImageExtent.height);

        return actual;
    }
}
