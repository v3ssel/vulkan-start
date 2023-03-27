#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <iostream>

#include "VulkanManager/vulkan_manager.h"
#include "DisplayWindow/display_window.h"

int main() {
    mvk::Triangle t;

    try {
        t.Run();
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }
    
    return 0;
}
