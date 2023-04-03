cmake_minimum_required(VERSION 3.5)
project(MVK LANGUAGES C CXX)

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

set(SHADERS_SOURCES
    Shaders/ShadersHelper.cpp
)

add_library(shaders_lib STATIC ${SHADERS_SOURCES})
target_link_libraries(shaders_lib PUBLIC
        shaderc_combined
        glslang
        OSDependent
        OGLCompiler
        SPIRV
        HLSL
        SPIRV-Tools-opt
        SPIRV-Tools
)

set(SOURCES 
    main.cpp
    VulkanManager/VulkanManager.cpp 	
    VulkanValidator/VulkanValidator.cpp
    QueueFamilies/QueueFamilies.cpp  	   
    SwapChainDetails/SwapChainDetails.cpp 
    DisplayWindow/DisplayWindow.cpp
)

add_executable(MVK ${SOURCES})
target_link_libraries(MVK ${Vulkan_LIBRARIES} glfw3 shaders_lib)
