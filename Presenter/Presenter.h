#ifndef MVK_PRESENTER
#define MVK_PRESENTER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include "../VulkanManager/VulkanManager.h"
#include "../ObjectLoader/ObjectLoader.h"

namespace mvk {
    class VKPresenter : public VulkanManager {
       public:
        void Setup(GLFWwindow* window);
        void DrawFrame();
        void RecordCommandBuffer(vk::CommandBuffer command_buffer, uint32_t image_index);
        void UpdateUniforms(uint32_t current_image);
        void PrintLoadedData();

        void set_window_resize();
       
       private:
        uint32_t current_frame_ = 0;
        bool window_resized_ = false;
        ObjectLoader loader_;
       
    };
}

#endif  // MVK_PRESENTER