#ifndef MVK_SHADERS_HELPER
#define MVK_SHADERS_HELPER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>

#include <iostream>
#include <string>
#include <fstream>

#include "../MVKConstants.h"

namespace mvk {
    class ShadersHelper {
       public:
        static std::string ReadFromFile(const std::string file_name);
        static vk::ShaderModuleCreateInfo LoadVertexShader();
        static vk::ShaderModuleCreateInfo LoadFragmentShader();
        static std::vector<uint32_t> LoadShader(const std::string file_name, const shaderc_shader_kind kind, const std::string name);
};
}

#endif  // MVK_SHADERS_HELPER
