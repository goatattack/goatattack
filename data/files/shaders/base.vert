#version 330

uniform mat4 projection_matrix;

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 texuv;
layout (location = 2) in vec4 color;

out vec2 tex_coord;
out vec4 out_color;

void main() {
    gl_Position = projection_matrix * vec4(vertex, 0.0, 1.0);
    tex_coord = texuv;
    out_color = color;
}
