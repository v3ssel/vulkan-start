#include "ShadersHelper.h"

namespace mvk {
    std::string mvk::ShadersHelper::ReadFromFile(const std::string file_name) {
        std::ifstream file(file_name);
        if (!file.is_open())
            throw std::runtime_error("Cannot open file: " + file_name);

        std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
        return content;
    }

    vk::ShaderModuleCreateInfo mvk::ShadersHelper::LoadVertexShader() {
        static auto vertex_code = LoadShader(VERTEX_SHADER_PATH, shaderc_vertex_shader, "VertexShader");

        vk::ShaderModuleCreateInfo vertex_info{};
        vertex_info.sType = vk::StructureType::eShaderModuleCreateInfo;
        vertex_info.setCodeSize(vertex_code.size() * sizeof(uint32_t));
        vertex_info.setPCode(vertex_code.data());

        return vertex_info;
    }

    vk::ShaderModuleCreateInfo mvk::ShadersHelper::LoadFragmentShader() {
        static auto fragment_code = LoadShader(FRAGMENT_SHADER_PATH, shaderc_fragment_shader, "FragmentShader");

        vk::ShaderModuleCreateInfo fragment_info{};
        fragment_info.sType = vk::StructureType::eShaderModuleCreateInfo;
        fragment_info.setCodeSize(fragment_code.size() * sizeof(uint32_t));
        fragment_info.setPCode(fragment_code.data());

        return fragment_info;
    }

    std::vector<uint32_t> ShadersHelper::LoadShader(const std::string file_name, const shaderc_shader_kind kind, const std::string name) {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetOptimizationLevel(shaderc_optimization_level_size);

        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(ShadersHelper::ReadFromFile(file_name), kind, name.c_str(), options);

        if (module.GetCompilationStatus() != shaderc_compilation_status_success)
            throw std::runtime_error("Shaders cannot be compiled. :" + file_name);
        
        return std::vector<uint32_t>(module.cbegin(), module.cend());
    }
}
