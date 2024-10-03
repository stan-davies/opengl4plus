#version 410

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_colour;
layout(location = 2) in vec2 texture_coords;

out vec3 colour;
out vec2 tex_coord;

void main() {
        colour = vertex_colour;
        tex_coord = texture_coords;
        // fourth component is used for perspective, 1.0 means no perspective
        gl_Position = vec4(vertex_position, 1.0);
}