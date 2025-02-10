#ifndef meshObject_hpp
#define meshObject_hpp

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.hpp>
#include <map>

class meshObject {
public:
    meshObject();
    ~meshObject();

    void draw(const glm::mat4& view, const glm::mat4& projection);
    void drawPicking(const glm::mat4& view, const glm::mat4& projection);
    void translate(const glm::vec3& translation); // Translate the object
    void rotate(float angle, const glm::vec3& axis); // Rotate the object

    int getId() const { return id; } // Getter for the ID
    
    static meshObject* getMeshObjectById(int id); // Retrieve object by ID
    
    // TODO: P1bTask4 - Create a list of children.

private:
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram;
    GLuint pickingShaderProgram;
    glm::mat4 modelMatrix;

    static int nextId; // Static counter for unique IDs
    int id;            // ID for this specific object
    
    GLsizei numIndices;
    
    static std::map<int, meshObject*> meshObjectMap; // Static map of ID to Object
};

#endif
