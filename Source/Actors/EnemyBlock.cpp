//
// Created by dti Digital on 21/06/2025.
//

//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "EnemyBlock.h"
#include "../Game.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"

EnemyBlock::EnemyBlock(Game *game,
                       const std::string &texturePath,
                       const bool isStatic)
    : Actor(game)
{
    constexpr float spikeColliderWidth = Game::TILE_SIZE - 15.0f;
    constexpr float spikeColliderHeight = Game::TILE_SIZE - 20.0f;
    constexpr float offsetX = 7.5f;
    constexpr float offsetY = 10;

    new DrawSpriteComponent(this,
                            texturePath,
                            Game::TILE_SIZE,
                            Game::TILE_SIZE,
                            10);

    mColliderComponent = new AABBColliderComponent(this,
                                                   offsetX,
                                                   offsetY,
                                                   spikeColliderWidth,
                                                   spikeColliderHeight,
                                                   ColliderLayer::EnemyBlocks,
                                                   isStatic);

    mRigidBodyComponent = new RigidBodyComponent(this,
                                                 1.0f,
                                                 0.0f,
                                                 false);
}



