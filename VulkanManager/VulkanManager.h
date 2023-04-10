#ifndef MVK_MANAGER
#define MVK_MANAGER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>


#include "../MVKConstants.h"
#include "VulkanObjects.h"
#include "../Shaders/ShadersHelper.h"
#include "../GraphicsSettings/GraphicsSettings.h"

namespace mvk {
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

#endif  // MVK_MANAGER
