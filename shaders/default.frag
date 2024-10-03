#version 410

uniform sampler2D tex1;
uniform sampler2D tex2;

in vec3 colour;
in vec2 tex_coord;

out vec4 frag_colour;

void main() {
        vec2 reversed_coord = vec2(1.0 - tex_coord.x, tex_coord.y);
        frag_colour = mix(texture(tex1, tex_coord), texture(tex2, reversed_coord), 0.2);
}