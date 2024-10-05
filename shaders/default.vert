#version 410

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 texture_coords;

out vec2 tex_coord;

void main() {
        tex_coord = texture_coords;
        // fourth component is used for perspective, 1.0 means no perspective
        gl_Position = vec4(vertex_position, 1.0);
}