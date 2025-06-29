//
// Created by dti Digital on 28/06/2025.
//

#include "RobotPlane.h"
#include "RobotPlane.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
RobotPlane::RobotPlane(Game* game, const float forwardSpeed, const float liftSpeed)
        : Actor(game)
        , mForwardSpeed(forwardSpeed)
        , mLiftSpeed(liftSpeed)
{
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f);

    mColliderComponent = new AABBColliderComponent(
        this, 0.0f, 0.0f,
        Game::TILE_SIZE, Game::TILE_SIZE,
        ColliderLayer::Player
    );

    mDrawComponent = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/ByteGuardPlane/ByteGuardPlane.png",
        "../Assets/Sprites/ByteGuardPlane/ByteGuardPlane.json",
        200
    );

    mDrawComponent->AddAnimation("fly", {2});
    mDrawComponent->AddAnimation("idle", {1});
    mDrawComponent->AddAnimation("down", {0});
    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);

    mRigidBodyComponent->SetApplyGravity(true);
}

void RobotPlane::OnProcessInput(const uint8_t* state)
{
    if (mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

    if (state[SDL_SCANCODE_SPACE])
    {
        mRigidBodyComponent->SetVelocity(Vector2(
            mRigidBodyComponent->GetVelocity().x,
            mLiftSpeed
        ));

        mDrawComponent->SetAnimation("fly");
    }
    else
    {
        mDrawComponent->SetAnimation("idle");
    }
}

void RobotPlane::OnUpdate(float deltaTime)
{
    mRigidBodyComponent->SetVelocity(Vector2(
        mForwardSpeed,
        mRigidBodyComponent->GetVelocity().y
    ));

    if (mPosition.y > mGame->GetWindowHeight())
    {
        Kill();
    }
}

void RobotPlane::Kill()
{
    mState = ActorState::Destroy;
    mGame->SetGamePlayState(Game::GamePlayState::GameOver);

    mGame->GetAudio()->PlaySound("ByteGuard_Dead.ogg");
    mGame->ResetGameScene(0.5f);
}
