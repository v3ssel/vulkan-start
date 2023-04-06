#ifndef VULKAN_MANAGER
#define VULKAN_MANAGER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>

#include <chrono>
#include <random>

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
        "VK_LAYER_LUNARG_monitor"
    };

    const std::vector<const char*> DEVICE_REQUIRED_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        "VK_EXT_extended_dynamic_state3"
    };

    constexpr uint32_t MAX_FRAMES = 2;

    class VulkanManager {
       public:
        void CreateInstance();
        void SetupDebug();
        void CreateSurface(GLFWwindow *window);
        void TakeVideocard();
        void CreateLogicalDevice();
        void CreateSwapChain(vk::SwapchainKHR *prev = nullptr);
        void RecreateSwapChain();
        void CreateImageView();
        void CreateRenderPass();
        void CreateGraphicsPipeline();
        void CreateFramebuffers();
        void CreateCommandPool();
        void CreateCommandBuffers();
        void CreateSyncObjects();

        void DrawFrame();

        void DestroyEverything();
        void DestroySwapchain();
        void PrintLoadedData();

        vk::Device& get_logical_device();
        void set_window_resize();
         
       private:
        void FillDebugInfo(vk::DebugUtilsMessengerCreateInfoEXT &debug_info);
        void RecordCommandBuffer(vk::CommandBuffer, uint32_t image_index);
        
        GLFWwindow *window_;
        mvk::VulkanObjects vo_;
        float r = 0.0f, g = 0.0f, b = 0.0f;
        uint32_t current_frame_ = 0;
        bool window_resized_ = false;
    };
}

#endif  // VULKAN_MANAGER
