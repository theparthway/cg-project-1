#include "projectile.hpp"

Projectile::Projectile(const std::string& objPath, const glm::vec3& startPos, const glm::vec3& initialVelocity) 
    : position(startPos), velocity(initialVelocity), hitGround(false) {
    mesh = new meshObject(objPath);
}

Projectile::~Projectile() {
    delete mesh;
}

void Projectile::update(float deltaTime) {
    if (hitGround) return;
    
    position += velocity * deltaTime;
    
    velocity.y -= gravity * deltaTime;
    
    if (position.y <= 0.0f) {
        position.y = 0.0f;
        hitGround = true;
    }
    
    mesh->modelMatrix = glm::mat4(1.0f);
    mesh->translate(position);
    
    // Add rotation for visual effect
    mesh->rotate(deltaTime * 90.0f, glm::vec3(1.0f, 1.0f, 0.0f));
}

void Projectile::draw(const glm::mat4& view, const glm::mat4& projection, const LightingInfo& lightingInfo) {
    mesh->drawWithChildren(view, projection, glm::mat4(1.0f), -1, lightingInfo);
}

bool Projectile::hasHitGround() const {
    return hitGround;
}

glm::vec3 Projectile::getPosition() const {
    return position;
}