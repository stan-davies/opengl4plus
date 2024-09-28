#version 420

uniform vec4 input_colour;
out vec4 frag_colour;

void main() {
    frag_colour = input_colour;
}