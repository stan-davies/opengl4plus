#version 460

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 texture_coords;

out vec2 img_coord;

void main() {
        img_coord = texture_coords;
        // fourth component is used for perspective, 1.0 means no perspective
        gl_Position = vec4(vertex_position, 0.0, 1.0);
}