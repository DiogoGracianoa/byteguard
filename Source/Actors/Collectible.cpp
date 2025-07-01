//
// Created by lucas on 29/06/2025.
//

#include "Collectible.h"

#include "../Game.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"

Collectible::Collectible(Game *game,
                         Powerups powerupType,
                         const std::string &imagePath)
    : Actor(game),
      mPowerupType(powerupType)
{
    mColliderComponent = new AABBColliderComponent(this,
                                                   0,
                                                   0,
                                                   Game::TILE_SIZE,
                                                   Game::TILE_SIZE,
                                                   ColliderLayer::Collectible,
                                                   true);

    mColliderComponent->SetIsSensor(true);

    new DrawSpriteComponent(this,
                            imagePath,
                            Game::TILE_SIZE - 10,
                            Game::TILE_SIZE - 10,
                            10);
}

void Collectible::Kill()
{
    mState = ActorState::Destroy;
    mGame->GetAudio()->PlaySound("Pick_powerup.wav");
}
