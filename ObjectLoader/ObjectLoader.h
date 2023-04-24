#ifndef MVK_OBJ_LOADER
#define MVK_OBJ_LOADER

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include <array>
#include <fstream>
#include <sstream>

#include "../MVKConstants.h"

namespace mvk {
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Color;
        glm::vec2 UVs;
    };

    struct MVP {
        glm::mat4 Model;
        glm::mat4 View;
        glm::mat4 Projection;
    };

    class ObjectLoader {
       public:
        void LoadObject();

        static vk::VertexInputBindingDescription GetVerticesBindingDescription();
        static std::vector<vk::VertexInputAttributeDescription> GetVerticesAttributeDescription();

        std::vector<Vertex> object;
    };
}

#endif  // MVK_OBJ_LOADER
