#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <iostream>

#include "VulkanManager/VulkanManager.h"
#include "DisplayWindow/DisplayWindow.h"

int main() {
    mvk::DisplayWindow t;

    try {
        t.Run();
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }
    
    return 0;
}
