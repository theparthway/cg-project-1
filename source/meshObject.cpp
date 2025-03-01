#include "meshObject.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

// Initialize static member
int meshObject::nextId = 1;
std::map<int, meshObject*> meshObject::meshObjectMap;

// me: P1bTask4
void meshObject::addChild(meshObject* child) {
    children.push_back(child);
    child->setParent(this);
}

void meshObject::setParent(meshObject* newParent) {
    parent = newParent;
}

void meshObject::drawWithChildren(const glm::mat4& view, const glm::mat4& projection, 
                                  const glm::mat4 parentModel, int selectedId,
                                  const LightingInfo& lightingInfo) {
    // Combine with parent's model matrix
    glm::mat4 combinedModel = parentModel * modelMatrix;

    bool isSelected = (id == selectedId);
    
    // Draw this object using the combined model matrix
    glUseProgram(shaderProgram);
    
    // Compute the MVP matrix
    glm::mat4 MVP = projection * view * combinedModel;
    GLuint matrixID = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(MVP));

    // Pass model matrix to shader for lighting calculations
    GLuint modelMatrixID = glGetUniformLocation(shaderProgram, "modelMatrix");
    glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(combinedModel));
    
    // Pass view matrix to shader
    GLuint viewMatrixID = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, glm::value_ptr(view));

    // Pass lighting info to shader
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPosition"), 1, glm::value_ptr(lightingInfo.cameraPosition));
    
    // Light 1
    glUniform3fv(glGetUniformLocation(shaderProgram, "light1Position"), 1, glm::value_ptr(lightingInfo.light1Position));
    glUniform3fv(glGetUniformLocation(shaderProgram, "light1Ambient"), 1, glm::value_ptr(lightingInfo.light1Ambient));
    glUniform3fv(glGetUniformLocation(shaderProgram, "light1Diffuse"), 1, glm::value_ptr(lightingInfo.light1Diffuse));
    glUniform3fv(glGetUniformLocation(shaderProgram, "light1Specular"), 1, glm::value_ptr(lightingInfo.light1Specular));
    
    // Light 2
    glUniform3fv(glGetUniformLocation(shaderProgram, "light2Position"), 1, glm::value_ptr(lightingInfo.light2Position));
    glUniform3fv(glGetUniformLocation(shaderProgram, "light2Ambient"), 1, glm::value_ptr(lightingInfo.light2Ambient));
    glUniform3fv(glGetUniformLocation(shaderProgram, "light2Diffuse"), 1, glm::value_ptr(lightingInfo.light2Diffuse));
    glUniform3fv(glGetUniformLocation(shaderProgram, "light2Specular"), 1, glm::value_ptr(lightingInfo.light2Specular));
    
    // Material properties
    glUniform3fv(glGetUniformLocation(shaderProgram, "materialAmbient"), 1, glm::value_ptr(lightingInfo.materialAmbient));
    glUniform3fv(glGetUniformLocation(shaderProgram, "materialDiffuse"), 1, glm::value_ptr(lightingInfo.materialDiffuse));
    glUniform3fv(glGetUniformLocation(shaderProgram, "materialSpecular"), 1, glm::value_ptr(lightingInfo.materialSpecular));
    glUniform1f(glGetUniformLocation(shaderProgram, "materialShininess"), lightingInfo.materialShininess);

    // Selected flag
    GLuint selectedID = glGetUniformLocation(shaderProgram, "isSelected");
    glUniform1i(selectedID, isSelected ? 1 : 0);
    
    // Draw the object
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    // Draw all children with the combined model matrix
    for (auto child : children) {
        child->drawWithChildren(view, projection, combinedModel, selectedId, lightingInfo);
    }
}

// TODO: P1aTask3 - Modify the constructor of Object to accept the path to an obj file
meshObject::meshObject(const std::string& objFilePath) : id(nextId++) { // Assign current value of nextId to id and increment it
    
    // Add this object to the map
    meshObjectMap[id] = this;

    // Initialize the model matrix
    modelMatrix = glm::mat4(1.0f);

    // Generate and bind VAO, VBO, and EBO
    // Generate and bind VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &NBO);

    glBindVertexArray(VAO);

    // TODO: P1aTask3 - Read from obj file instead.
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "../";

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(objFilePath, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            vertices.push_back(attrib.vertices[3 * index.vertex_index]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            indices.push_back(indices.size());
        }
    }
    
    //TODO: P1bTask5 - Create normal buffer.
    std::vector<GLfloat> normals;
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            normals.push_back(attrib.normals[3 * index.normal_index]);
            normals.push_back(attrib.normals[3 * index.normal_index + 1]);
            normals.push_back(attrib.normals[3 * index.normal_index + 2]);
        }
    }

    // Bind normals to NBO
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);

    // Set up vertex attribute pointers for normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);
    
    numIndices = (GLsizei)indices.size();
    std::cout << numIndices << std::endl;
    
    // Bind vertices to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Bind normals to NBO
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);

    // Bind element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    // Load and compile shaders
    shaderProgram = LoadShaders("meshVertexShader.glsl", "meshFragmentShader.glsl");
    pickingShaderProgram = LoadShaders("pickingVertexShader.glsl", "pickingFragmentShader.glsl");
}

meshObject::~meshObject() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &NBO);
    glDeleteProgram(shaderProgram);
    
    // Remove this object from the map
    meshObjectMap.erase(id);
}

//TODO: P1bTask5 - Modify to accept lighiting info as arguement.
void meshObject::draw(const glm::mat4& view, const glm::mat4& projection, bool selected) {
    glUseProgram(shaderProgram);
    
    // Compute the MVP matrix
    glm::mat4 MVP = projection * view * modelMatrix;
    GLuint matrixID = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(MVP));
    
    //TODO: P1bTask5 - Send lighting info to shader using uniform. May also need to send the model matrix seperatily as a uniform.

    // me: P1bTask4
    GLuint selectedID = glGetUniformLocation(shaderProgram, "isSelected");
    glUniform1i(selectedID, selected ? 1 : 0);

    // Draw the object
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void meshObject::translate(const glm::vec3& translation) {
    // Apply translation to the model matrix
    modelMatrix = glm::translate(modelMatrix, translation);
}

void meshObject::rotate(float angle, const glm::vec3& axis) {
    // Apply rotation to the model matrix
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), axis);
}

void meshObject::drawPicking(const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(pickingShaderProgram); // Use the picking shader

    //TODO: P1bBonus2 - Send the value of the id variable to the shader as a uniform. Use glUniform1f

    glm::mat4 MVP = projection * view * modelMatrix;
    GLuint matrixID = glGetUniformLocation(pickingShaderProgram, "MVP");
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(MVP));

    // Draw the object
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0); // Unbind the shader program
}

meshObject* meshObject::getMeshObjectById(int id) {
    // Look up the object by ID in the map
    auto it = meshObjectMap.find(id);
    if (it != meshObjectMap.end()) {
        return it->second;
    }
    return nullptr; // Return nullptr if ID not found
}
