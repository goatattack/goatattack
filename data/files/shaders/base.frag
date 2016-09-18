#version 330

uniform sampler2D tex;

in vec2 tex_coord;
in vec4 out_color;

layout(location = 0) out vec4 fragment_color;

void main() {
    fragment_color = texture(tex, tex_coord) * out_color;
}
