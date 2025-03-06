#version 330 core

// Input vertex attributes (from VBO)
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

// Uniforms
uniform mat4 MVP;

void main() {
    // Transform the vertex position
    gl_Position = MVP * vec4(position, 1.0);
}
