//
// Created by lucas on 29/06/2025.
//

#pragma once

#include <string>
#include "Actor.h"
#include "Player.h"

class Collectible final : public Actor
{
public:
    explicit Collectible(Game *game,
                         Powerups powerupType,
                         const std::string &imagePath);

    void Kill() override { mState = ActorState::Destroy; };

    Powerups GetPowerupType() const {return mPowerupType;}

private:
    class AABBColliderComponent *mColliderComponent;
    Powerups mPowerupType;
};
