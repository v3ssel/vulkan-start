#ifndef MVK_CONSTANTS
#define MVK_CONSTANTS

#include <vulkan/vulkan.hpp>

namespace mvk {
    constexpr int WIDTH = 1280;
    constexpr int HEIGHT = 720;

    #ifdef NDEBUG
        constexpr bool ENABLE_VALIDATION_LAYERS = false;
    #else
        constexpr bool ENABLE_VALIDATION_LAYERS = true;
    #endif

    const std::string VERTEX_SHADER_PATH = "C:\\Coding\\Projects\\VulkanTesting\\Shaders\\VertexShader.glsl";
    const std::string FRAGMENT_SHADER_PATH = "C:\\Coding\\Projects\\VulkanTesting\\Shaders\\FragmentShader.glsl";

    const std::vector<const char*> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_LUNARG_monitor"
    };

    const std::vector<const char*> DEVICE_REQUIRED_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        "VK_EXT_extended_dynamic_state3"
    };

    const std::vector<const char*> INSTANCE_REQUIRED_EXTENSIONS = {
        "VK_KHR_get_physical_device_properties2"
    };

    const std::vector<vk::DynamicState> DYNAMIC_STATES = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
        vk::DynamicState::ePolygonModeEXT
    };

    constexpr uint32_t MAX_FRAMES = 2;
}

#endif  // MVK_CONSTANTS
