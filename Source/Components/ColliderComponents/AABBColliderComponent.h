//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once
#include <map>
#include <SDL_render.h>
#include <set>
#include <vector>
#include "../Component.h"
#include "../RigidBodyComponent.h"
#include "../../Math.h"

enum class ColliderLayer
{
    Player,
    Enemy,
    Blocks,
    Pole,
    Collectible,
    EnemyBlocks,
};

class AABBColliderComponent final : public Component
{
public:
    // Collider ignore map
    const std::map<ColliderLayer, const std::set<ColliderLayer>>
    ColliderIgnoreMap = {
            {ColliderLayer::Player, {}},
            {ColliderLayer::Enemy, {}},
            {ColliderLayer::Blocks, {ColliderLayer::Blocks}},
            {ColliderLayer::Pole, {}},
            {
            ColliderLayer::Collectible, {
            ColliderLayer::Blocks,
            ColliderLayer::Enemy,
            ColliderLayer::Collectible
            }
            }
            };

    AABBColliderComponent(class Actor *owner,
                          int dx,
                          int dy,
                          int w,
                          int h,
                          ColliderLayer layer,
                          bool isStatic = false,
                          int updateOrder = 10);

    ~AABBColliderComponent() override;

    bool Intersect(const AABBColliderComponent &b) const;

    float DetectHorizontalCollision(RigidBodyComponent *rigidBody) const;

    float DetectVerticalCollision(RigidBodyComponent *rigidBody) const;

    void SetStatic(const bool isStatic) { mIsStatic = isStatic; }

    void SetIsSensor(const bool isSensor) { mIsSensor = isSensor; }
    bool IsSensor() const { return mIsSensor; }

    Vector2 GetMin() const;

    Vector2 GetMax() const;

    Vector2 GetCenter() const;

    ColliderLayer GetLayer() const { return mLayer; }

    // Novo método para alterar dimensões dinamicamente
    void SetDimensions(const int w, const int h)
    {
        mWidth = w;
        mHeight = h;
    }

    // Novo método para alterar apenas o offset (caso precise reposicionar)
    void SetOffset(const int dx, const int dy) { mOffset = Vector2(dx, dy); }

    void DrawDebug(SDL_Renderer *renderer) const;

private:
    float GetMinVerticalOverlap(const AABBColliderComponent *b) const;

    float GetMinHorizontalOverlap(const AABBColliderComponent *b) const;

    void ResolveHorizontalCollisions(RigidBodyComponent *rigidBody,
                                     float minXOverlap) const;

    void ResolveVerticalCollisions(RigidBodyComponent *rigidBody,
                                   float minYOverlap) const;

    Vector2 mOffset;
    int mWidth;
    int mHeight;
    bool mIsStatic;
    bool mIsSensor = false;

    ColliderLayer mLayer;
};
