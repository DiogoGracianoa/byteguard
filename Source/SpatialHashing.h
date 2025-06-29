//
// Created by Lucas N. Ferreira on 29/05/25.
//

#pragma once

#include <unordered_map>
#include <vector>
#include "Math.h"
#include "Actors/Actor.h"

class SpatialHashing
{
public:
    SpatialHashing(int cellSize, int width, int height);

    ~SpatialHashing();

    void Insert(Actor *actor);

    void Remove(Actor *actor);

    void Reinsert(Actor *actor);

    std::vector<AABBColliderComponent *> QueryColliders(
        const Vector2 &position,
        int range = 1) const;

    std::vector<Actor *> Query(const Vector2 &position,
                               int range = 1) const;

    std::vector<Actor *> QueryOnCamera(const Vector2 &cameraPosition,
                                       float screenWidth,
                                       float screenHeight,
                                       float extraRadius = 0.0f) const;

private:
    int mCellSize;
    int mWidth;
    int mHeight;

    // 2D grid of colliders
    std::vector<std::vector<std::vector<Actor *> > > mGrid;
    // Maps collider to its position
    std::unordered_map<Actor *, Vector2> mPositions;
    // Maps collider to its grid cell indices
    std::unordered_map<Actor *, std::pair<int, int> > mCellIndices;
};
