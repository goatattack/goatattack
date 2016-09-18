#version 330

uniform vec4 color;
uniform sampler2D tex;

in vec2 tex_coord;

layout(location = 0) out vec4 fragment_color;

void main() {
    fragment_color = texture(tex, tex_coord) * color;
}
