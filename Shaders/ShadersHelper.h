#ifndef SHADERS_HELPER
#define SHADERS_HELPER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>

#include <iostream>
#include <string>
#include <fstream>

namespace mvk {
    const std::string VERTEX_SHADER_PATH = "C:\\Coding\\Projects\\VulkanTesting\\Shaders\\VertexShader.glsl";
    const std::string FRAGMENT_SHADER_PATH = "C:\\Coding\\Projects\\VulkanTesting\\Shaders\\FragmentShader.glsl";

    class ShadersHelper {
       public:
        static std::string ReadFromFile(const std::string file_name);
        static vk::ShaderModuleCreateInfo LoadVertexShader();
        static vk::ShaderModuleCreateInfo LoadFragmentShader();
        static std::vector<uint32_t> LoadShader(const std::string file_name, const shaderc_shader_kind kind, const std::string name);
};
}

#endif // SHADERS_HELPER
