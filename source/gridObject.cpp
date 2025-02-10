#include "gridObject.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>


gridObject::gridObject() { // Assign current value of nextId to id and increment it
    
    // Initialize the model matrix
    modelMatrix = glm::mat4(1.0f);

    // Generate and bind VAO, VBO, and EBO
    // Generate and bind VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // TODO: P1aTask1 - Modify vertices and indices to draw grid and axis lines. Note: multiple vertices at the origin will be needed due to different color of axis lines.
    
    // Tetrahedron vertices with positions and colors
    std::vector<GLfloat> vertices = {
        // Positions          // Colors
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // origin red
        5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // x red

        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // origin green
        0.0f, 5.0f, 0.0f, 0.0f, 1.0f, 0.0f, // y green

        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // origin blue
        0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 1.0f, // z blue
    };

    // z lines
    for (int i=-5;i<=5;i++) {
        vertices.push_back(-5.0f); vertices.push_back(0.0f); vertices.push_back(i);
        vertices.push_back(1.0f); vertices.push_back(1.0f); vertices.push_back(1.0f);

        vertices.push_back(5.0f); vertices.push_back(0.0f); vertices.push_back(i);
        vertices.push_back(1.0f); vertices.push_back(1.0f); vertices.push_back(1.0f);
    }

    // x lines
    for (int i=-5;i<=5;i++) {
        vertices.push_back(i); vertices.push_back(0.0f); vertices.push_back(-5.0f);
        vertices.push_back(1.0f); vertices.push_back(1.0f); vertices.push_back(1.0f);

        vertices.push_back(i); vertices.push_back(0.0f); vertices.push_back(5.0f);
        vertices.push_back(1.0f); vertices.push_back(1.0f); vertices.push_back(1.0f);
    }

    // Tetrahedron indices
    std::vector<GLuint> indices = {
    };

    for (int i=0;i<vertices.size();i++) {
        indices.push_back(i);
    }
    
    numIndices = (GLsizei)indices.size();
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Load and compile shaders
    shaderProgram = LoadShaders("gridVertexShader.glsl", "gridFragmentShader.glsl");
}

gridObject::~gridObject() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    
}

void gridObject::draw(const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);
    
    // TODO: P1aTask1 - Compute the MVP matrix and send to shader as a uniform. Refer to meshObject.cpp as reference.
    glm::mat4 MVP = projection * view * modelMatrix;
    GLuint matrixID = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(MVP));
    
    // TODO: P1aTask1 - Bind the VAO and draw the grid using glDrawElements(GL_LINES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(VAO);
    glDrawElements(GL_LINES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
