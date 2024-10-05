#version 410

uniform sampler2D tex1;

in vec2 tex_coord;

out vec4 frag_colour;

void main() {
        frag_colour = texture(tex1, tex_coord);
}