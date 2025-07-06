//
// Created by lucas on 28/06/2025.
//

#pragma once

#include "Actor.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

class Missile final : public Actor
{
public:
    explicit Missile(Game *game,
                     float dForceFactor = 330.0f,
                     float sForceFactor = 5.0f);

    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(float minOverlap,
                               AABBColliderComponent *other) override;

    void OnVerticalCollision(float minOverlap,
                             AABBColliderComponent *other) override;

    void Kill() override;

private:
    RigidBodyComponent *mRigidBodyComponent;
    AABBColliderComponent *mColliderComponent;
    DrawAnimatedComponent *mDrawComponent;

    // movement parameters

    float mDForceFactor;
    float mSForceFactor;
};
