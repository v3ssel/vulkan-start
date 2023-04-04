#ifndef VULKAN_MANAGER
#define VULKAN_MANAGER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>

#include "VulkanObjects.h"
#include "../Shaders/ShadersHelper.h"
#include "../GraphicsSettings/GraphicsSettings.h"

namespace mvk {
    #ifdef NDEBUG
        constexpr bool ENABLE_VALIDATION_LAYERS = false;
    #else
        constexpr bool ENABLE_VALIDATION_LAYERS = true;
    #endif

    const std::vector<const char*> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation",
    };

    const std::vector<const char*> DEVICE_REQUIRED_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    class VulkanManager {
       public:
        void CreateInstance();
        void SetupDebug();
        void CreateSurface(GLFWwindow *window);
        void TakeVideocard();
        void CreateLogicalDevice();
        void CreateSwapChain(GLFWwindow *window);
        void CreateImageView();
        void CreateRenderPass();
        void CreateGraphicsPipeline();
        void CreateFramebuffers();
        void CreateCommandPool();
        void CreateCommandBuffer();

        void DestroyEverything();
        void PrintLoadedData();
         
       private:
        void FillDebugInfo(vk::DebugUtilsMessengerCreateInfoEXT &debug_info);
        void RecordCommandBuffer(vk::CommandBuffer, uint32_t image_index);
        
        mvk::VulkanObjects vo_;
    };
}

#endif  // VULKAN_MANAGER
