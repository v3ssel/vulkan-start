#version 460

layout(binding = 1) uniform sampler2D texture_sampler;

layout(location = 0) in vec3 FragColor;
layout(location = 1) in vec2 FragTexPos;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(texture(texture_sampler, FragTexPos).rgb, 1.0);
}
