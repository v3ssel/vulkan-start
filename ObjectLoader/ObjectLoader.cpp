#include "ObjectLoader.h"

void mvk::ObjectLoader::LoadObject() {
    std::ifstream f(OBJECT_PATH);
    
    std::string line;
    while (std::getline(f, line)) {
        if (line == "") continue;
        std::istringstream ss(line);
        
        std::vector<double> nums;

        double num;
        while(ss >> num)
            nums.push_back(num);

        object.push_back({{nums[0], nums[1], nums[2]}, {nums[3], nums[4], nums[5]}, {nums[6], nums[7]}});
    }
}

vk::VertexInputBindingDescription mvk::ObjectLoader::GetVerticesBindingDescription()
{
    vk::VertexInputBindingDescription vertex_binding_desc{};
    vertex_binding_desc.setBinding(0);
    vertex_binding_desc.setStride(sizeof(Vertex));
    vertex_binding_desc.setInputRate(vk::VertexInputRate::eVertex);
    return vertex_binding_desc;
}

std::vector<vk::VertexInputAttributeDescription> mvk::ObjectLoader::GetVerticesAttributeDescription() {
    std::vector<vk::VertexInputAttributeDescription> vertex_attributes(3);
    vertex_attributes[0].setBinding(0);
    vertex_attributes[0].setLocation(0);
    vertex_attributes[0].setFormat(vk::Format::eR32G32B32Sfloat);
    vertex_attributes[0].setOffset(offsetof(Vertex, Vertex::Position));

    vertex_attributes[1].setBinding(0);
    vertex_attributes[1].setLocation(1);
    vertex_attributes[1].setFormat(vk::Format::eR32G32B32Sfloat);
    vertex_attributes[1].setOffset(offsetof(Vertex, Vertex::Color));

    vertex_attributes[2].setBinding(0);
    vertex_attributes[2].setLocation(2);
    vertex_attributes[2].setFormat(vk::Format::eR32G32Sfloat);
    vertex_attributes[2].setOffset(offsetof(Vertex, Vertex::UVs));

    return vertex_attributes;
}
