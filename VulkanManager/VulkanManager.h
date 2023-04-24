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
        
        void CreateImageViews();
        void CreateRenderPass();
        void CreateDescriptorSetLayout();
        void CreateGraphicsPipeline();
        void CreateFramebuffers();
        void CreateCommandPool();

        void CreateTextureImage();
        void CreateTextureImageView();
        void CreateTextureSampler();
        
        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateUniformBuffers();
        void CreateDescriptorPool();
        void CreateDescriptorSets();


        void CreateCommandBuffers();
        void CreateSyncObjects();
        void CreateObject();

        void DestroyEverything();

        vk::Device& get_logical_device();

       protected: 
        virtual void DrawFrame() {}
        virtual void RecordCommandBuffer(vk::CommandBuffer, uint32_t image_index) {}

        mvk::VulkanObjects vo_;
       
       private:
        vk::ImageView CreateImageView(vk::Image image, vk::Format format);

        void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &memory);
        void CopyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size);

        void CreateImage(uint32_t width, uint32_t heigth, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image &image, vk::DeviceMemory &memory);
        void TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout old_layout, vk::ImageLayout new_layout);
        void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

        vk::CommandBuffer BeginSingletimeCommand();
        void EndSingletimeCommand(vk::CommandBuffer cmd_buffer);

        void FillDebugInfo(vk::DebugUtilsMessengerCreateInfoEXT &debug_info);
        void DestroySwapchainImages();
        GLFWwindow *window_;
    };
}

#endif  // MVK_MANAGER
