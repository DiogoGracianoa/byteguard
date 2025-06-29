//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Mario.h"
#include "Block.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Mario::Mario(Game* game, const float forwardSpeed, const float jumpSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsOnPole(false)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
        , mJumpSpeed(jumpSpeed)
        , mPoleSlideTimer(0.0f)
{
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);
    mRigidBodyComponent->SetVelocity(Vector2(mForwardSpeed, .0f));

    float byteGuardColliderWidth = Game::TILE_SIZE - 15.0f;
    float byteGuardColliderHeight = Game::TILE_SIZE;
    float offsetX = 7.5f;
    float offsetY = 0;

    mColliderComponent = new AABBColliderComponent(this, offsetX, offsetY, byteGuardColliderWidth,byteGuardColliderHeight,
                                                   ColliderLayer::Player);

    mDrawComponent = new DrawAnimatedComponent(this,
                                              "../Assets/Sprites/ByteGuard/ByteGuard.png",
                                              "../Assets/Sprites/ByteGuard/ByteGuard.json", 200);

    mDrawComponent->AddAnimation("Dead", {0});
    mDrawComponent->AddAnimation("idle", {0});
    mDrawComponent->AddAnimation("jump", {0,1,2,3,4,5,6,7});
    mDrawComponent->AddAnimation("run", {9,10,11,12,13});
    mDrawComponent->AddAnimation("win", {0});


    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);

}

void Mario::OnProcessInput(const uint8_t* state)
{
    if(mGame->GetGamePlayState() != Game::GamePlayState::Playing) return;

    if (state[SDL_SCANCODE_D])
    {
        mRigidBodyComponent->ApplyForce(Vector2::UnitX * mForwardSpeed * 4.0f);
        mRotation = 0.0f;
        mIsRunning = true;
    }

    if (state[SDL_SCANCODE_A])
    {
        mRigidBodyComponent->ApplyForce(Vector2::UnitX * -mForwardSpeed * 4.0f);
        mRotation = Math::Pi;
        mIsRunning = true;
    }

    if (!state[SDL_SCANCODE_D] && !state[SDL_SCANCODE_A])
    {
        mIsRunning = false;
    }

    if (state[SDL_SCANCODE_SPACE] && mIsOnGround)
    {
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed));
        mIsOnGround = false;

        mGame->GetAudio()->PlaySound("ByteGuard_Jump.ogg");
    }
}



void Mario::OnUpdate(float deltaTime)
{
    // Check if Mario is off the ground
    if (mRigidBodyComponent && mRigidBodyComponent->GetVelocity().y != 0)
    {
        mIsOnGround = false;
    }

    mRigidBodyComponent->ApplyForce(Vector2::UnitX * mForwardSpeed * 5);

    if (mGame->GetGamePlayState() == Game::GamePlayState::Playing &&
        mPosition.x < mGame->GetCameraPos().x) { Kill(); }

    // Kill mario if he falls below the screen
    if (mGame->GetGamePlayState() == Game::GamePlayState::Playing &&
        mPosition.y > mGame->GetWindowHeight()) { Kill(); }



    ManageAnimations();
}

void Mario::ManageAnimations()
{
    if(mIsDying)
    {
        mDrawComponent->SetAnimation("Dead");
    }
    else if(mIsOnPole)
    {
        mDrawComponent->SetAnimation("win");
    }
    else if (mIsOnGround && mIsRunning)
    {
        mDrawComponent->SetAnimation("run");
    }
    else if (mIsOnGround && !mIsRunning)
    {
        mDrawComponent->SetAnimation("idle");
    }
    else if (!mIsOnGround)
    {
        mDrawComponent->SetAnimation("jump");
    }
}

void Mario::Kill()
{
    mIsDying = true;
    mGame->SetGamePlayState(Game::GamePlayState::GameOver);
    mDrawComponent->SetAnimation("Dead");

    // Disable collider and rigid body
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    mGame->GetAudio()->StopAllSounds();
    mGame->GetAudio()->PlaySound("ByteGuard_Dead.ogg");

    mGame->ResetGameScene(1.0f);
}

void Mario::Win(AABBColliderComponent *poleCollider)
{
    mDrawComponent->SetAnimation("win");
    mGame->SetGamePlayState(Game::GamePlayState::LevelComplete);

    // Set mario velocity to go down
    mRigidBodyComponent->SetVelocity(Vector2::UnitY * 100.0f); // 100 pixels per second
    mRigidBodyComponent->SetApplyGravity(false);

    // Disable collider
    poleCollider->SetEnabled(false);

    // Adjust mario x position to grab the pole
    mPosition.Set(poleCollider->GetOwner()->GetPosition().x + Game::TILE_SIZE / 4.0f, mPosition.y);

    // --------------
    // TODO - PARTE 4
    // --------------

    // TODO 1.: Pare todos os sons com StopAllSounds()
    mGame->GetAudio()->StopAllSounds();

    mPoleSlideTimer = POLE_SLIDE_TIME; // Start the pole slide timer
}

void Mario::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy || other->GetLayer() == ColliderLayer::EnemyBlocks)
    {
        Kill();
    }
    /*else if (other->GetLayer() == ColliderLayer::Pole)
    {
        mIsOnPole = true;
        Win(other);
    }*/
    else if (other->GetLayer() == ColliderLayer::Collectible)
    {
        mGame->AddCoin();
        other->SetEnabled(false);
        other->GetOwner()->SetState(ActorState::Destroy);
    }
}

void Mario::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        other->GetOwner()->Kill();
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed / 2.5f));

        // --------------
        // TODO - PARTE 4
        // --------------


    }
    else if (other->GetLayer() == ColliderLayer::Blocks)
    {
        if (!mIsOnGround)
        {
            // --------------
            // TODO - PARTE 4
            // --------------


            // Cast actor to Block to call OnBump
            auto* block = dynamic_cast<Block*>(other->GetOwner());
            //block->OnBump();
        }
    }
    else if (other->GetLayer() == ColliderLayer::Collectible)
    {
        mGame->AddCoin();
        other->GetOwner()->SetState(ActorState::Destroy);
    }
    if (other->GetLayer() == ColliderLayer::EnemyBlocks)
    {
        Kill();
    }
}