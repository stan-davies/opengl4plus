#version 410

uniform float time;

in vec3 colour;
out vec4 frag_colour;

void main() {
        vec3 mult = vec3(sin(-time), (sin(time)), time);
        frag_colour = vec4(colour * mult, 1.0);
}