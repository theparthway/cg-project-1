#version 330 core

// Input vertex attributes (from VBO)
layout(location = 0) in vec3 position; // Vertex position
//TODO: P1bTask5 - Input Normals for lighting
layout(location = 1) in vec3 normal;

// Uniforms
uniform mat4 MVP; // Combined Model-View-Projection matrix
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

// output to fragment shader
out vec3 FragPos;
out vec3 Normal;

void main() {
    // Transform the vertex position
    gl_Position = MVP * vec4(position, 1.0);

    FragPos = vec3(modelMatrix * vec4(position, 1.0));

    Normal = mat3(transpose(inverse(modelMatrix))) * normal;
}
