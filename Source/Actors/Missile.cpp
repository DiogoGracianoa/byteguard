//
// Created by lucas on 28/06/2025.
//

#include "Missile.h"

#include "ExplosionParticles.h"
#include "Player.h"
#include "../Game.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"

Missile::Missile(Game *game, const float dForceFactor, const float sForceFactor)
    : Actor(game),
      mDForceFactor(dForceFactor),
      mSForceFactor(sForceFactor)

{
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

    mDrawComponent = new DrawAnimatedComponent(this,
                                               "../Assets/Sprites/Missile/Missile.png",
                                               "../Assets/Sprites/Missile/Missile.json");

    mDrawComponent->AddAnimation("seek", {0, 1, 2});
    // mDrawComponent->AddAnimation("explode", {3, 4});

    mDrawComponent->SetAnimation("seek");
    mDrawComponent->SetAnimFPS(10.0f);
}

void Missile::OnUpdate(const float deltaTime)
{
    if (mGame == nullptr) {
        Kill();
        return;
    }

    const auto player = mGame->GetPlayer();
    if (player == nullptr) {
        Kill();
        return;
    }

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
    if (other->GetLayer() == ColliderLayer::Enemy) { return; }

    Kill();
}

void Missile::OnVerticalCollision(const float minOverlap,
                                  AABBColliderComponent *other)
{
    if (other->GetLayer() == ColliderLayer::Player)
    {
        other->GetOwner()->Kill();
    }
    if (other->GetLayer() == ColliderLayer::Enemy) { return; }

    Kill();
}

void Missile::Kill()
{
    mState = ActorState::Destroy;
    mGame->GetAudio()->PlaySound("Missile_Explosion.ogg");
    int numParticles = 15;
    for (int i = 0; i < numParticles; i++)
    {
        new ExplosionParticles(mGame, mPosition, 1.0f);
    }
}
