#ifndef DISPLAY_WINDOW
#define DISPLAY_WINDOW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "../Presenter/Presenter.h"

constexpr int WIDTH = 1280;
constexpr int HEIGHT = 720;

namespace mvk {
    class DisplayWindow {
       public:
        void Run();
        void SetResizeTrigger();

       private:
        void InitWindow();
        void InitVulkan();
        void MainLoop();
        void CleanUp();

        GLFWwindow* window_;
        mvk::VKPresenter screen; 
    };
}

#endif  // DISPLAY_WINDOW
