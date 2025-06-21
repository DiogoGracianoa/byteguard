//
// Created by dti Digital on 21/06/2025.
//

#pragma once

#include "Actor.h"
#include <string>

class EnemyBlock : public Actor
{
public:
    explicit EnemyBlock(Game* game, const std::string &texturePath, const bool isStatic = true);

    void SetPosition(const Vector2& position)
    {
        Actor::SetPosition(position);
        mOriginalPosition.Set(position.x, position.y);
    }
private:

    Vector2 mOriginalPosition;

    class AABBColliderComponent* mColliderComponent;
    class RigidBodyComponent* mRigidBodyComponent;
};
