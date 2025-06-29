//
// Created by Diogo Graciano on 14/06/2025.
//

#include "Collectible.h"
#include "../Game.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"

Collectible::Collectible(Game *game)
    : Actor(game)
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
                            "../Assets/Sprites/Collectables/Coin.png",
                            Game::TILE_SIZE,
                            Game::TILE_SIZE,
                            10);
}
