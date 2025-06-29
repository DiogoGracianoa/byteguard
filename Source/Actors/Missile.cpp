//
// Created by lucas on 28/06/2025.
//

#include "Missile.h"
#include "Mario.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"

Missile::Missile(Game *game, const float dForceFactor, const float sForceFactor)
    : Actor(game)
, mDForceFactor(dForceFactor)
, mSForceFactor(sForceFactor)

{
    new DrawSpriteComponent(this,
        "../Assets/Sprites/Missile/missile.png",
        Game::TILE_SIZE,
        Game::TILE_SIZE,
                            10);

    mRigidBodyComponent = new RigidBodyComponent(this, 1, 0, false);

    mColliderComponent = new AABBColliderComponent(
        this, 0, 0, Game::TILE_SIZE / 2, Game::TILE_SIZE / 2,
        ColliderLayer::Enemy);
}

void Missile::OnUpdate(float deltaTime)
{
    auto player = mGame->GetMario();
    auto dForce =
            mDForceFactor * Vector2::Normalize(player->GetPosition() - GetPosition());
    auto sForce = mSForceFactor * (dForce - mRigidBodyComponent->GetVelocity());
    mRigidBodyComponent->ApplyForce(sForce);
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

void Missile::Kill()
{
    mState = ActorState::Destroy;
}
