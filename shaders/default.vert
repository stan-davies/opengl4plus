#version 420

in vec3 vertex_position;

void main() {
    // fourth component is used for perspective, 1.0 means no perspective
    gl_Position = vec4(vertex_position, 1.0);
}