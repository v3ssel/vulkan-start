#ifndef MVK_CONSTANTS
#define MVK_CONSTANTS

#include <vulkan/vulkan.hpp>
#include "ObjectLoader/ObjectLoader.h"

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
    const std::string TEXTURE_IMAGE_PATH = "C:\\Coding\\Projects\\VulkanTesting\\obamna\\obamna.jpg";
    const std::string OBJECT_PATH = "C:\\Coding\\Projects\\VulkanTesting\\obamna\\obamna.txt";

    const std::vector<const char*> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_LUNARG_monitor"
    };

    const std::vector<const char*> DEVICE_REQUIRED_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        "VK_EXT_extended_dynamic_state3"
    };

    const std::vector<const char*> INSTANCE_REQUIRED_EXTENSIONS = {
        "VK_KHR_get_physical_device_properties2",
        "VK_EXT_validation_features"
    };

    const std::vector<vk::DynamicState> DYNAMIC_STATES = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
        vk::DynamicState::ePolygonModeEXT
    };

    constexpr uint32_t MAX_FRAMES = 2;
    
    // const std::vector<Vertex> VERTICES = {
    //     {{-1.281770, -1.018835,  1.287239}, {1.0, 0.0, 1.0}, {0.196212, 0.507752}},
    //     {{ 0.002734,  0.992958,  0.002735}, {0.0, 1.0, 0.0}, {0.047232, 0.072788}},
    //     {{-1.281770, -1.018835, -1.281770}, {0.0, 1.0, 1.0}, {0.504819, 0.224520}},
        
    //     {{-1.281770, -1.018835, -1.281770}, {0.0, 1.0, 1.0}, {0.487756, 0.816358}},
    //     {{ 0.002734,  0.992958,  0.002735}, {0.0, 1.0, 0.0}, {0.036699, 0.946886}},
    //     {{ 1.287239, -1.018835, -1.281770}, {1.0, 0.0, 0.0}, {0.195675, 0.513964}},
        
    //     {{ 1.287239, -1.018835, -1.281770}, {1.0, 0.0, 0.0}, {0.813871, 0.534227}},
    //     {{ 0.002734,  0.992958,  0.002735}, {0.0, 1.0, 0.0}, {0.956058, 0.967989}},
    //     {{ 1.287239, -1.018835,  1.287239}, {0.0, 0.0, 1.0}, {0.508838, 0.813238}},
        
    //     {{ 1.287239, -1.018835,  1.287239}, {0.0, 0.0, 1.0}, {0.518750, 0.227105}},
    //     {{ 0.002734,  0.992958,  0.002735}, {0.0, 1.0, 0.0}, {0.973437, 0.096012}},
    //     {{-1.281770, -1.018835,  1.287239}, {1.0, 0.0, 1.0}, {0.810937, 0.521418}},
        
    //     {{-1.281770, -1.018835, -1.281770}, {0.0, 1.0, 1.0}, {0.201562, 0.513294}},
    //     {{ 1.287239, -1.018835, -1.281770}, {1.0, 0.0, 0.0}, {0.514063, 0.235598}},
    //     {{ 1.287239, -1.018835,  1.287239}, {0.0, 0.0, 1.0}, {0.809375, 0.528434}},
        
    //     {{-1.281770, -1.018835, -1.281770}, {0.0, 1.0, 1.0}, {0.201562, 0.513294}},
    //     {{ 1.287239, -1.018835,  1.287239}, {0.0, 0.0, 1.0}, {0.809375, 0.528434}},
    //     {{-1.281770, -1.018835,  1.287239}, {1.0, 0.0, 1.0}, {0.504687, 0.810561}}
    // };



    const std::vector<uint32_t> INDICES = {
        0, 1, 2,
        3, 4, 5,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        15, 16, 17
    };
}

#endif  // MVK_CONSTANTS
