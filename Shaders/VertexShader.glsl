#version 460

layout(binding = 0) uniform MVP {
    mat4 Model;
    mat4 View;
    mat4 Projection;
} mvp;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

layout(location = 0) out vec3 FragColor;

void main() {
    gl_Position = mvp.Projection * mvp.View * mvp.Model * vec4(aPos, 1.0);
    gl_PointSize = 10.0;
    FragColor = aColor;
}
