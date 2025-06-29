//
// Created by dti Digital on 21/06/2025.
//

#pragma once

#include <string>
#include "Actor.h"

class EnemyBlock final : public Actor
{
public:
    explicit EnemyBlock(Game *game,
                        const std::string &texturePath,
                        const bool isStatic = true);

private:
    Vector2 mOriginalPosition;

    class AABBColliderComponent *mColliderComponent;
    class RigidBodyComponent *mRigidBodyComponent;
};
