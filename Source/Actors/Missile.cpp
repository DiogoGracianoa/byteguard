//
// Created by lucas on 28/06/2025.
//

#include "Missile.h"
#include "Player.h"
#include "../Game.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"

Missile::Missile(Game *game, const float dForceFactor, const float sForceFactor)
    : Actor(game),
      mDForceFactor(dForceFactor),
      mSForceFactor(sForceFactor)

{
    new DrawSpriteComponent(this,
                            "../Assets/Sprites/Missile/missile.png",
                            Game::TILE_SIZE,
                            Game::TILE_SIZE,
                            10);

    mRigidBodyComponent = new RigidBodyComponent(this,
                                                 1.0f,
                                                 0.0f,
                                                 false);

    mColliderComponent = new AABBColliderComponent(this,
                                                   0.0f,
                                                   0.0f,
                                                   Game::TILE_SIZE / 2,
                                                   Game::TILE_SIZE / 2,
                                                   ColliderLayer::Enemy);
}

void Missile::OnUpdate(const float deltaTime)
{
    const auto player = mGame->GetPlayer();
    const auto dForce =
            mDForceFactor * Vector2::Normalize(
                player->GetPosition() - GetPosition()
            );

    const auto sForce =
            mSForceFactor * (dForce - mRigidBodyComponent->GetVelocity());

    mRigidBodyComponent->ApplyForce(sForce);
    const auto vel = mRigidBodyComponent->GetVelocity();
    mRotation = Math::Atan2(vel.y, vel.x);
}

void Missile::OnHorizontalCollision(const float minOverlap,
                                    AABBColliderComponent *other)
{
    if (other->GetLayer() == ColliderLayer::Player)
    {
        other->GetOwner()->Kill();
    }

    mState = ActorState::Destroy;
}

void Missile::OnVerticalCollision(const float minOverlap,
                                  AABBColliderComponent *other)
{
    if (other->GetLayer() == ColliderLayer::Player)
    {
        other->GetOwner()->Kill();
    }

    mState = ActorState::Destroy;
}

void Missile::Kill() { mState = ActorState::Destroy; }
