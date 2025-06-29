//
// Created by Lucas N. Ferreira on 08/09/23.
//

#include "RigidBodyComponent.h"
#include "../Game.h"
#include "../Actors/Actor.h"
#include "ColliderComponents/AABBColliderComponent.h"

constexpr float MAX_SPEED_X = 750.0f * 1.25f;
constexpr float MAX_SPEED_Y = 750 * 1000000.0f;
constexpr float GRAVITY = 2500.0f;

RigidBodyComponent::RigidBodyComponent(class Actor *owner,
                                       const float mass,
                                       const float friction,
                                       const bool applyGravity,
                                       const int updateOrder)
    : Component(owner, updateOrder),
      mApplyGravity(applyGravity),
      mApplyFriction(true),
      mFrictionCoefficient(friction),
      mMass(mass),
      mVelocity(Vector2::Zero),
      mAcceleration(Vector2::Zero) {}

void RigidBodyComponent::ApplyForce(const Vector2 &force)
{
    mAcceleration += force * (1.f / mMass);
}

void RigidBodyComponent::Update(const float deltaTime)
{
    // Apply gravity acceleration
    if (mApplyGravity) { ApplyForce(Vector2::UnitY * GRAVITY); }

    // Apply friction
    if (mApplyFriction && Math::Abs(mVelocity.x) > 0.05f)
    {
        ApplyForce(Vector2::UnitX * -mFrictionCoefficient * mVelocity.x);
    }

    // Euler Integration
    mVelocity += mAcceleration * deltaTime;

    mVelocity.x = Math::Clamp<float>(mVelocity.x,
                                     -MAX_SPEED_X,
                                     MAX_SPEED_X);

    mVelocity.y = Math::Clamp<float>(mVelocity.y,
                                     -MAX_SPEED_Y,
                                     MAX_SPEED_Y);

    if (Math::NearZero(mVelocity.x, 1.0f)) { mVelocity.x = 0.f; }

    const auto collider = mOwner->GetComponent<AABBColliderComponent>();

    if (mVelocity.x != 0.0f)
    {
        mOwner->SetPosition(Vector2(
            mOwner->GetPosition().x + mVelocity.x * deltaTime,
            mOwner->GetPosition().y));

        if (collider) { collider->DetectHorizontalCollision(this); }
    }

    if (mVelocity.y != 0.0f)
    {
        mOwner->SetPosition(Vector2(mOwner->GetPosition().x,
                                    mOwner->GetPosition().y + mVelocity.y *
                                    deltaTime));

        if (collider) { collider->DetectVerticalCollision(this); }
    }

    mAcceleration.Set(0.f, 0.f);
}

