//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include "Actor.h"
#include <string>

#include "Actor.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

class PressMachine : public Actor
{
public:
    PressMachine(class Game* game, SDL_Renderer* renderer);

protected:
    void OnUpdate(float deltaTime) override;

private:
    class DrawAnimatedComponent* mAnim;
    class AABBColliderComponent* mColliderComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    SDL_Renderer* mRenderer;
};
