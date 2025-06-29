//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AABBColliderComponent.h"
#include <algorithm>
#include "../../Game.h"
#include "../../Actors/Actor.h"

AABBColliderComponent::AABBColliderComponent(class Actor *owner,
                                             const int dx,
                                             const int dy,
                                             const int w,
                                             const int h,
                                             const ColliderLayer layer,
                                             const bool isStatic,
                                             const int updateOrder)
    : Component(owner, updateOrder),
      mOffset(Vector2(static_cast<float>(dx),
                      static_cast<float>(dy))),
      mWidth(w),
      mHeight(h),
      mIsStatic(isStatic),
      mLayer(layer)
{
    //    mOwner->GetGame()->AddCollider(this);
}

AABBColliderComponent::~AABBColliderComponent()
{
    //    mOwner->GetGame()->RemoveCollider(this);
}

Vector2 AABBColliderComponent::GetMin() const
{
    return mOwner->GetPosition() + mOffset;
}

Vector2 AABBColliderComponent::GetMax() const
{
    return GetMin() + Vector2(static_cast<float>(mWidth),
                              static_cast<float>(mHeight));
}

Vector2 AABBColliderComponent::GetCenter() const
{
    return GetMin() + Vector2(static_cast<float>(mWidth) / 2.0f,
                              static_cast<float>(mHeight) / 2.0f);
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent &b) const
{
    return (GetMin().x < b.GetMax().x && GetMax().x > b.GetMin().x &&
            GetMin().y < b.GetMax().y && GetMax().y > b.GetMin().y);
}

float AABBColliderComponent::GetMinVerticalOverlap(
    const AABBColliderComponent *b) const
{
    const float top = GetMin().y - b->GetMax().y; // Top
    const float down = GetMax().y - b->GetMin().y; // Down

    return (Math::Abs(top) < Math::Abs(down)) ? top : down;
}

float AABBColliderComponent::GetMinHorizontalOverlap(
    const AABBColliderComponent *b) const
{
    const float right = GetMax().x - b->GetMin().x; // Right
    const float left = GetMin().x - b->GetMax().x; // Left

    return (Math::Abs(left) < Math::Abs(right)) ? left : right;
}

float AABBColliderComponent::DetectHorizontalCollision(
    RigidBodyComponent *rigidBody) const
{
    if (mIsStatic || !mIsEnabled) return false;

    // Use spatial hashing to get nearby colliders
    auto colliders =
            mOwner->GetGame()->GetNearbyColliders(mOwner->GetPosition());

    std::sort(colliders.begin(),
              colliders.end(),
              [this](const AABBColliderComponent *a,
                     const AABBColliderComponent *b)
              {
                  return Math::Abs(
                      (a->GetCenter() - GetCenter()).LengthSq() < (
                          b->GetCenter() - GetCenter()).LengthSq());
              }
    );

    for (const auto &collider: colliders)
    {
        if (collider == this || !collider->IsEnabled()) continue;

        // Check if the collider is in the same layer or if it should be ignored
        if (ColliderIgnoreMap.at(mLayer).find(collider->GetLayer()) !=
            ColliderIgnoreMap.at(mLayer).end())
        {
            continue; // Ignore this collider
        }

        if (Intersect(*collider))
        {
            const float minHorizontalOverlap =
                    GetMinHorizontalOverlap(collider);

            if (!IsSensor() && !collider->IsSensor())
            {
                ResolveHorizontalCollisions(rigidBody, minHorizontalOverlap);
            }

            mOwner->OnHorizontalCollision(minHorizontalOverlap, collider);
            return minHorizontalOverlap;
        }
    }

    return 0.0f;
}

float AABBColliderComponent::DetectVerticalCollision(
    RigidBodyComponent *rigidBody) const
{
    if (mIsStatic || !mIsEnabled) return false;

    // Use spatial hashing to get nearby colliders
    auto colliders =
            mOwner->GetGame()->GetNearbyColliders(mOwner->GetPosition());

    std::sort(colliders.begin(),
              colliders.end(),
              [this](const AABBColliderComponent *a,
                     const AABBColliderComponent *b)
              {
                  return Math::Abs(
                      (a->GetCenter() - GetCenter()).LengthSq() < (
                          b->GetCenter() - GetCenter()).LengthSq());
              }
    );

    for (const auto &collider: colliders)
    {
        if (collider == this || !collider->IsEnabled()) continue;

        // Check if the collider is in the same layer or if it should be ignored
        if (ColliderIgnoreMap.at(mLayer).find(collider->GetLayer()) !=
            ColliderIgnoreMap.at(mLayer).end())
        {
            continue; // Ignore this collider
        }

        if (Intersect(*collider))
        {
            const float minVerticalOverlap = GetMinVerticalOverlap(collider);
            if (!IsSensor() && !collider->IsSensor())
            {
                ResolveVerticalCollisions(rigidBody, minVerticalOverlap);
            }

            // Callback only for closest (first) collision
            mOwner->OnVerticalCollision(minVerticalOverlap, collider);
            return minVerticalOverlap;
        }
    }

    return 0.0f;
}

void AABBColliderComponent::ResolveHorizontalCollisions(
    RigidBodyComponent *rigidBody,
    const float minXOverlap) const
{
    mOwner->SetPosition(mOwner->GetPosition() - Vector2(minXOverlap, 0.0f));
    rigidBody->SetVelocity(Vector2(0.f, rigidBody->GetVelocity().y));
}

void AABBColliderComponent::ResolveVerticalCollisions(
    RigidBodyComponent *rigidBody,
    const float minYOverlap) const
{
    mOwner->SetPosition(mOwner->GetPosition() - Vector2(0.0f, minYOverlap));
    rigidBody->SetVelocity(Vector2(rigidBody->GetVelocity().x, 0.f));

    if (minYOverlap > .0f) { mOwner->SetOnGround(); }
}

void AABBColliderComponent::DrawDebug(SDL_Renderer *renderer) const
{
    const Vector2 pos = mOwner->GetPosition();
    SDL_Rect rect;
    rect.x = static_cast<int>(pos.x + mOffset.x - mWidth / 2.0f);
    rect.y = static_cast<int>(pos.y + mOffset.y - mHeight / 2.0f);
    rect.w = static_cast<int>(mWidth);
    rect.h = static_cast<int>(mHeight);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // verde
    SDL_RenderDrawRect(renderer, &rect);
}
