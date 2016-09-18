#version 330

uniform mat4 projection_matrix;
uniform vec2 offset;

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 texuv;

out vec2 tex_coord;

void main() {
    gl_Position = projection_matrix * vec4(vertex + offset, 0.0, 1.0);
    tex_coord = texuv;
}
