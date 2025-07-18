// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Actor.h"
#include <algorithm>
#include "../Game.h"
#include "../Components/Component.h"

Actor::Actor(Game *game)
    : mGame(game),
      mState(ActorState::Active),
      mPosition(Vector2::Zero),
      mScale(1.0f),
      mRotation(0.0f),
      mIsOnGround(false) { mGame->AddActor(this); }

Actor::~Actor()
{
    mGame->RemoveActor(this);

    for (const auto &component: mComponents) { delete component; }
    mComponents.clear();
}

void Actor::SetPosition(const Vector2 &pos)
{
    mPosition = pos;
    mGame->Reinsert(this);
}

void Actor::Update(const float deltaTime)
{
    if (mState == ActorState::Active)
    {
        for (const auto &comp: mComponents)
        {
            if (comp->IsEnabled()) { comp->Update(deltaTime); }
        }

        OnUpdate(deltaTime);
    }
}

void Actor::OnUpdate(float deltaTime) {}

void Actor::OnHorizontalCollision(const float minOverlap,
                                  AABBColliderComponent *other) {}

void Actor::OnVerticalCollision(const float minOverlap,
                                AABBColliderComponent *other) {}

void Actor::Kill() {}

void Actor::ProcessInput(const Uint8 *keyState)
{
    if (mState == ActorState::Active)
    {
        for (const auto &comp: mComponents) { comp->ProcessInput(keyState); }

        OnProcessInput(keyState);
    }
}

void Actor::HandleKeyPress(const int key, const bool isPressed)
{
    if (mState == ActorState::Active)
    {
        for (const auto &comp: mComponents)
        {
            comp->HandleKeyPress(key, isPressed);
        }

        // Call actor-specific key press handling
        OnHandleKeyPress(key, isPressed);
    }
}

void Actor::OnProcessInput(const Uint8 *keyState) {}

void Actor::OnHandleKeyPress(const int key, const bool isPressed) {}

void Actor::AddComponent(Component *c)
{
    mComponents.emplace_back(c);
    std::sort(
        mComponents.begin(),
        mComponents.end(),
        [](const Component *a, const Component *b)
        {
            return a->GetUpdateOrder() < b->GetUpdateOrder();
        });
}

bool Actor::IsVisibleOnCamera() const
{
    // Get the camera's position and dimensions
    const Vector2 cameraPosition = mGame->GetCameraPos();
    const float screenWidth = mGame->GetWindowWidth();
    const float screenHeight = mGame->GetWindowHeight();

    // Check if the actor's position is within the camera's view
    return (mPosition.x >= cameraPosition.x &&
            mPosition.x <= cameraPosition.x + screenWidth &&
            mPosition.y >= cameraPosition.y &&
            mPosition.y <= cameraPosition.y + screenHeight);
}
