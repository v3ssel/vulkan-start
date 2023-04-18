#ifndef MVK_OBJ_LOADER
#define MVK_OBJ_LOADER

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include <array>

namespace mvk {
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Color;
    };

    class ObjectLoader {
       public:
        ObjectLoader() = default;
        ObjectLoader(std::vector<Vertex> vertices);

        static vk::VertexInputBindingDescription GetVerticesBindingDescription();
        static std::array<vk::VertexInputAttributeDescription, 2> GetVerticesAttributeDescription();

        std::vector<Vertex> object;
    };
}

#endif  // MVK_OBJ_LOADER
