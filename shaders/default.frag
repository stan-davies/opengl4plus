#version 460

uniform sampler2D img;

in vec2 img_coord;

out vec4 frag_colour;

void main() {
        // frag_colour = vec4(1.0, 1.0, 1.0, 1.0);
        frag_colour = texture(img, img_coord);
}