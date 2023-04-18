#include "ObjectLoader.h"

mvk::ObjectLoader::ObjectLoader(std::vector<Vertex> vertices) {
    this->object = vertices;
}

vk::VertexInputBindingDescription mvk::ObjectLoader::GetVerticesBindingDescription() {
    vk::VertexInputBindingDescription vertex_binding_desc{};
    vertex_binding_desc.setBinding(0);
    vertex_binding_desc.setStride(sizeof(Vertex));
    vertex_binding_desc.setInputRate(vk::VertexInputRate::eVertex);
    return vertex_binding_desc;
}

std::array<vk::VertexInputAttributeDescription, 2> mvk::ObjectLoader::GetVerticesAttributeDescription() {
    std::array<vk::VertexInputAttributeDescription, 2> vertex_attributes{};
    vertex_attributes[0].setBinding(0);
    vertex_attributes[0].setLocation(0);
    vertex_attributes[0].setFormat(vk::Format::eR32G32B32Sfloat);
    vertex_attributes[0].setOffset(offsetof(Vertex, Vertex::Position));

    vertex_attributes[1].setBinding(0);
    vertex_attributes[1].setLocation(1);
    vertex_attributes[1].setFormat(vk::Format::eR32G32B32Sfloat);
    vertex_attributes[1].setOffset(offsetof(Vertex, Vertex::Color));

    return vertex_attributes;
}
