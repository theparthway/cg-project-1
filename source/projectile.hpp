#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "meshObject.hpp"

class Projectile {
public:
    Projectile(const std::string& objPath, const glm::vec3& startPos, const glm::vec3& initialVelocity);
    ~Projectile();
    
    void update(float deltaTime);
    void draw(const glm::mat4& view, const glm::mat4& projection, const LightingInfo& lightingInfo);
    bool hasHitGround() const;
    glm::vec3 getPosition() const;
    
private:
    meshObject* mesh;
    glm::vec3 position;
    glm::vec3 velocity;
    bool hitGround;
    const float gravity = 9.8f;
};

#endif // PROJECTILE_HPP