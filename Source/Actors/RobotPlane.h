//
// Created by dti Digital on 28/06/2025.
//

#pragma once
#include "Actor.h"

class RobotPlane : public Actor {
public:
    explicit RobotPlane(Game* game, float forwardSpeed = 260.0f, float liftSpeed = -180.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;

    void Kill() override;

private:
    float mForwardSpeed;
    float mLiftSpeed;

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};