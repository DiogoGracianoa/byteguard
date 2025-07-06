//
// Created by lucas on 05/07/2025.
//

#include "ExplosionParticles.h"

#include "../Random.h"
#include "../Components/DrawComponents/DrawComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"

ExplosionParticles::ExplosionParticles(Game *game,
                                       const Vector2 &position,
                                       const float deathTimer)
    : Actor(game),
      mDeathTimer(deathTimer),
      mDrawComponent(nullptr),
      mRigidBodyComponent(nullptr)
{
    SetPosition(position);

    const auto size = GenerateRandomSize();
    mDrawComponent = new DrawSpriteComponent(
        this,
        "../Assets/Sprites/Missile/Missile_Explosion2.png",
        size,
        size
    );

    mRigidBodyComponent = new RigidBodyComponent(this, 1, 0, false);

    const Vector2 randStartingForce = GenerateRandomStartingForce();
    mRigidBodyComponent->ApplyForce(randStartingForce);
}

float ExplosionParticles::GenerateRandomSize() const
{
    const float size = Random::GetFloatRange(0.3f, 0.6f);
    return size * Game::TILE_SIZE;
}

Vector2 ExplosionParticles::GenerateRandomStartingForce()
{
    float randDirX = Random::GetFloat() < 0.5 ? -1.0f : 1.0f;

    float randDirY = Random::GetFloat() < 0.5 ? -1.0f : 1.0f;

    float min = 1800.0f, max = 2800.0f;
    auto randForce = Random::GetVector(Vector2(0.0f, min),
                                       Vector2(max, max));

    randForce.Set(randForce.x * randDirX, randForce.y * randDirY);
    return randForce;
}

void ExplosionParticles::OnUpdate(float deltaTime)
{
    mDeathTimer -= deltaTime;
    if (mDeathTimer <= 0) { SetState(ActorState::Destroy); }
}
