//
// Created by dti Digital on 05/07/2025.
//

#pragma once

#include "Actor.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

class LaserSpikes final : public Actor
{
public:
    LaserSpikes(class Game *game, SDL_Renderer *renderer);

private:
    class DrawAnimatedComponent *mAnim;
    class AABBColliderComponent *mColliderComponent;
    class RigidBodyComponent *mRigidBodyComponent;
    SDL_Renderer *mRenderer;
};