#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

layout(location = 0) out vec3 FragColor;

void main() {
    gl_Position = vec4(aPos, 1.0);
    gl_PointSize = 10.0;
    FragColor = aColor;
}
// vec2 positions[3] = vec2[](
//     vec2( 0.0, -0.5, 0.0, 1.0, 0.0, 0.0),
//     vec2( 0.5,  0.5, 0.0, 0.0, 1.0, 0.0),
//     vec2(-0.5,  0.5, 0.0, 0.0, 0.0, 1.0)
// );

// vec3 colors[3] = vec3[](
//     vec3(1.0, 0.0, 0.0),
//     vec3(0.0, 1.0, 0.0),
//     vec3(0.0, 0.0, 1.0)
// );
