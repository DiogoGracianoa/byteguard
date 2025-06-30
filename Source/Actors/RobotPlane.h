//
// Created by dti Digital on 28/06/2025.
//

#pragma once

#include "Actor.h"
#include "../AudioSystem.h"

class RobotPlane : public Actor {
public:
    explicit RobotPlane(Game* game, float forwardSpeed = 260.0f, float liftSpeed = -180.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;

private:
    float mForwardSpeed;
    float mLiftSpeed;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};
