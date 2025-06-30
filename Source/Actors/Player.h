//
// Created by Lucas N. Ferreira on 08/09/23.
//

#pragma once
#include "Actor.h"
#include "../Components/PowerupComponents/PowerupComponent.h"

enum class Powerups
{
    TimePowerup
};

class Player final : public Actor
{
public:
    explicit Player(Game *game,
                    float forwardSpeed = 260.0f,
                    float jumpSpeed = -640.0f);

    void OnProcessInput(const Uint8 *keyState) override;

    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(float minOverlap,
                               AABBColliderComponent *other) override;

    void OnVerticalCollision(float minOverlap,
                             AABBColliderComponent *other) override;

    void Kill() override;

    void Win(AABBColliderComponent *poleCollider) const;

private:
    void ManageAnimations() const;

    float mForwardSpeed;
    float mJumpSpeed;
    bool mIsRunning;
    bool mIsDying;

    class RigidBodyComponent *mRigidBodyComponent;
    class DrawAnimatedComponent *mDrawComponent;
    class AABBColliderComponent *mColliderComponent;

    std::map<Powerups, PowerupComponent *> mCollectibles;
};
