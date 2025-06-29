//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include <string>
#include "Actor.h"

class Block final : public Actor
{
public:
    explicit Block(Game *game,
                   const std::string &texturePath,
                   const bool isStatic = true);

    void OnUpdate(float deltaTime) override;

    void OnBump();

private:
    const int BUMP_FORCE = 200;

    Vector2 mOriginalPosition;

    class AABBColliderComponent *mColliderComponent;
    class RigidBodyComponent *mRigidBodyComponent;
};
