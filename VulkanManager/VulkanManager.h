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
        "VK_LAYER_LUNARG_monitor"
    };

    const std::vector<const char*> DEVICE_REQUIRED_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        "VK_EXT_extended_dynamic_state3"
    };

    const std::vector<const char*> INSTANCE_REQUIRED_EXTENSIONS = {
        "VK_KHR_get_physical_device_properties2"
    };

    constexpr uint32_t MAX_FRAMES = 2;

    class VulkanManager {
       public:
        virtual ~VulkanManager() {}

        void CreateInstance();
        void SetupDebug();
        void CreateSurface(GLFWwindow *window);
        void TakeVideocard();
        void CreateLogicalDevice();
        void CreateSwapChain(bool prev = false);
        void RecreateSwapChain();
        void CreateImageView();
        void CreateRenderPass();
        void CreateGraphicsPipeline();
        void CreateFramebuffers();
        void CreateCommandPool();
        void CreateCommandBuffers();
        void CreateSyncObjects();

        void DestroyEverything();

        vk::Device& get_logical_device();

       protected: 
        virtual void DrawFrame() {}
        virtual void RecordCommandBuffer(vk::CommandBuffer, uint32_t image_index) {}

        mvk::VulkanObjects vo_;
       
       private:
        void FillDebugInfo(vk::DebugUtilsMessengerCreateInfoEXT &debug_info);
        void DestroySwapchainImages();
        GLFWwindow *window_;
    };
}

#endif  // VULKAN_MANAGER
