#ifndef meshObject_hpp
#define meshObject_hpp

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.hpp>
#include <map>
#include <tiny_obj_loader.h>
#include <string>
#include <vector>

struct LightingInfo {
    glm::vec3 cameraPosition;
    
    glm::vec3 light1Position;
    glm::vec3 light1Ambient;
    glm::vec3 light1Diffuse;
    glm::vec3 light1Specular;
    
    glm::vec3 light2Position;
    glm::vec3 light2Ambient;
    glm::vec3 light2Diffuse;
    glm::vec3 light2Specular;
    
    glm::vec3 materialAmbient;
    glm::vec3 materialDiffuse;
    glm::vec3 materialSpecular;
    float materialShininess;
};

class meshObject {
public:
    meshObject(const std::string& objFilePath);
    ~meshObject();

    void draw(const glm::mat4& view, const glm::mat4& projection, bool selected = false);
    void drawPicking(const glm::mat4& view, const glm::mat4& projection);
    void translate(const glm::vec3& translation); // Translate the object
    void rotate(float angle, const glm::vec3& axis); // Rotate the object

    int getId() const { return id; } // Getter for the ID
    
    static meshObject* getMeshObjectById(int id); // Retrieve object by ID
    
    // TODO: P1bTask4 - Create a list of children.
    std::vector<meshObject*> children;
    meshObject* parent = nullptr;

    void addChild(meshObject* child);
    void setParent(meshObject* parent);
    void drawWithChildren(const glm::mat4& view, const glm::mat4& projection, glm::mat4 parentModel = glm::mat4(1.0f), int selectedId = -1, const LightingInfo& lightingInfo = {});
    glm::mat4 modelMatrix;

private:
    GLuint VAO, VBO, EBO, NBO;
    GLuint shaderProgram;
    GLuint pickingShaderProgram;

    static int nextId; // Static counter for unique IDs
    int id;            // ID for this specific object
    
    GLsizei numIndices;
    
    static std::map<int, meshObject*> meshObjectMap; // Static map of ID to Object
};

#endif
