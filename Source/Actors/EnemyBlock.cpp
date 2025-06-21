//
// Created by dti Digital on 21/06/2025.
//

//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "EnemyBlock.h"
#include "../Game.h"
#include "../Actors/Goomba.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"

EnemyBlock::EnemyBlock(Game* game, const std::string &texturePath, const bool isStatic)
        :Actor(game)
{
    float spikeColliderWidth = Game::TILE_SIZE - 10.0f;
    float spikeColliderHeight = Game::TILE_SIZE - 10.0f;
    float offsetX = 0;
    float offsetY = 0;

    new DrawSpriteComponent(this, texturePath, Game::TILE_SIZE, Game::TILE_SIZE, 10);
    mColliderComponent = new AABBColliderComponent(this, offsetX, offsetY, spikeColliderWidth, spikeColliderHeight, ColliderLayer::EnemyBlocks, isStatic);
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false);

}



