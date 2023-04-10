#ifndef MVK_DISPLAY_WINDOW
#define MVK_DISPLAY_WINDOW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "../Presenter/Presenter.h"
#include "../MVKConstants.h"

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

#endif  // MVK_DISPLAY_WINDOW
