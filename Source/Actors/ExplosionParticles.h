//
// Created by lucas on 05/07/2025.
//

#pragma once

#include "Actor.h"
#include "../Game.h"


class ExplosionParticles : public Actor
{
public:
    ExplosionParticles(Game *game,
                       const Vector2 &position,
                       float deathTimer = 2.0f);

    void OnUpdate(float deltaTime) override;

private:
    float GenerateRandomSize() const;

    static Vector2 GenerateRandomStartingForce();

    float mDeathTimer;

    class DrawSpriteComponent *mDrawComponent;
    class RigidBodyComponent *mRigidBodyComponent;
};
