//
// Created by Diogo Graciano on 14/06/2025.
//

#pragma once

#include "Actor.h"

class Collectible final : public Actor
{
public:
    explicit Collectible(Game *game);

private:
    class AABBColliderComponent *mColliderComponent;
};
