//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Player.h"
#include "Block.h"
#include "Collectible.h"
#include "../Components/PowerupComponents/TimePowerupComponent.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Player::Player(Game *game, const float forwardSpeed, const float jumpSpeed)
    : Actor(game),
      mForwardSpeed(forwardSpeed),
      mJumpSpeed(jumpSpeed),
      mIsRunning(false),
      mIsDying(false)
{
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);
    mRigidBodyComponent->SetVelocity(Vector2(mForwardSpeed, .0f));

    constexpr float byteGuardColliderWidth = Game::TILE_SIZE - 15.0f;
    constexpr float byteGuardColliderHeight = Game::TILE_SIZE;
    constexpr float offsetX = 7.5f;
    constexpr float offsetY = 0;

    mColliderComponent = new AABBColliderComponent(
        this, offsetX, offsetY, byteGuardColliderWidth, byteGuardColliderHeight,
        ColliderLayer::Player);

    mDrawComponent = new DrawAnimatedComponent(this,
                                               "../Assets/Sprites/ByteGuard/ByteGuard.png",
                                               "../Assets/Sprites/ByteGuard/ByteGuard.json",
                                               200);

    mDrawComponent->AddAnimation("Dead", {0});
    mDrawComponent->AddAnimation("idle", {0});
    mDrawComponent->AddAnimation("jump", {0, 1, 2, 3, 4, 5, 6, 7});
    mDrawComponent->AddAnimation("run", {9, 10, 11, 12, 13});
    mDrawComponent->AddAnimation("win", {0});


    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);

    mCollectibles = {
            {Powerups::TimePowerup, new TimePowerupComponent(this, 5.0f, 2.0f)}
            };
}

void Player::OnProcessInput(const uint8_t *state)
{
    if (mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

    if (state[SDL_SCANCODE_D])
    {
        mRigidBodyComponent->ApplyForce(Vector2::UnitX * mForwardSpeed * 4.0f);
        mRotation = 0.0f;
        mIsRunning = true;
        mDrawComponent->SetAnimFPS(20.0f);
    }

    if (state[SDL_SCANCODE_A])
    {
        mRigidBodyComponent->ApplyForce(Vector2::UnitX * -mForwardSpeed * 4.0f);
        mRotation = Math::Pi;
        mIsRunning = true;
    }

    if (!state[SDL_SCANCODE_D] && !state[SDL_SCANCODE_A])
    {
        mRotation = 0.0f;
        mIsRunning = true;
        mDrawComponent->SetAnimFPS(10.0f);
    }

    if (state[SDL_SCANCODE_SPACE] && mIsOnGround)
    {
        mRigidBodyComponent->SetVelocity(
            Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed));
        mIsOnGround = false;

        mGame->GetAudio()->PlaySound("ByteGuard_Jump.ogg");
    }

    if (state[SDL_SCANCODE_Q])
    {
        if (mCollectibles[Powerups::TimePowerup]->IsUsable())
        {
            const auto powerup =
                    dynamic_cast<TimePowerupComponent *>(
                        mCollectibles[Powerups::TimePowerup]
                    );

            powerup->ReleasePower();
        };
    }
}


void Player::OnUpdate(float deltaTime)
{
    // Check if Player is off the ground
    if (mRigidBodyComponent && mRigidBodyComponent->GetVelocity().y != 0)
    {
        mIsOnGround = false;
    }

    mRigidBodyComponent->ApplyForce(Vector2::UnitX * mForwardSpeed * 5);

    if (mGame->GetGamePlayState() == Game::GamePlayState::Playing &&
        mPosition.x < mGame->GetCameraPos().x) { Kill(); }

    // Kill player if he falls below the screen
    if (mGame->GetGamePlayState() == Game::GamePlayState::Playing &&
        mPosition.y > mGame->GetWindowHeight()) { Kill(); }


    ManageAnimations();
}

void Player::ManageAnimations() const
{
    if (mIsDying) { mDrawComponent->SetAnimation("Dead"); }
    else if (mIsOnGround && mIsRunning) { mDrawComponent->SetAnimation("run"); }
    else if (mIsOnGround && !mIsRunning)
    {
        mDrawComponent->SetAnimation("idle");
    }
    else if (!mIsOnGround) { mDrawComponent->SetAnimation("jump"); }
}

void Player::Kill()
{
    mIsDying = true;
    mGame->SetGamePlayState(Game::GamePlayState::GameOver);
    mDrawComponent->SetAnimation("Dead");

    // Disable collider and rigid body
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    mGame->GetAudio()->StopAllSounds();
    mGame->GetAudio()->PlaySound("ByteGuard_Dead.ogg");

    mCollectibles[Powerups::TimePowerup]->Disable();
    mCollectibles[Powerups::TimePowerup]->SetUsable(false);
    mCollectibles[Powerups::TimePowerup]->SetEnabled(false);

    mGame->ResetGameScene(1.0f);
}

void Player::Win(AABBColliderComponent *poleCollider) const
{
    mDrawComponent->SetAnimation("win");
    mGame->SetGamePlayState(Game::GamePlayState::LevelComplete);
}

void Player::OnHorizontalCollision(const float minOverlap,
                                   AABBColliderComponent *other)
{
    if (other->GetLayer() == ColliderLayer::Enemy || other->GetLayer() ==
        ColliderLayer::EnemyBlocks) { Kill(); }
    else if (other->GetLayer() == ColliderLayer::Collectible)
    {
        if (const auto collectible = dynamic_cast<Collectible *>(other->
                GetOwner());
            collectible &&
            collectible->GetPowerupType() == Powerups::TimePowerup &&
            !mCollectibles[Powerups::TimePowerup]->IsUsable())
        {
            mCollectibles[Powerups::TimePowerup]->SetUsable(true);
        }
        other->SetEnabled(false);
        other->GetOwner()->SetState(ActorState::Destroy);
    }
}

void Player::OnVerticalCollision(const float minOverlap,
                                 AABBColliderComponent *other)
{
    if (other->GetLayer() == ColliderLayer::Enemy || other->GetLayer() ==
        ColliderLayer::EnemyBlocks) { Kill(); }
    else if (other->GetLayer() == ColliderLayer::Collectible)
    {
        if (const auto collectible = dynamic_cast<Collectible *>(other->
                GetOwner());
            collectible &&
            collectible->GetPowerupType() == Powerups::TimePowerup &&
            !mCollectibles[Powerups::TimePowerup]->IsUsable())
        {
            mCollectibles[Powerups::TimePowerup]->SetUsable(true);
        }
        other->SetEnabled(false);
        other->GetOwner()->SetState(ActorState::Destroy);
    }
}
