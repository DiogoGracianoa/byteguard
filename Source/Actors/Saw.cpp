//
// Created by dti Digital on 05/07/2025.
//

#include "Saw.h"

Saw::Saw(Game *game, SDL_Renderer *renderer)
    : Actor(game),
      mRenderer(renderer)

{
    mAnim = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/Saw/saw.png",
        "../Assets/Sprites/Saw/saw.json"
    );
    std::vector<int> frames = {0, 1, 2, 3, 4, 5};
    mAnim->AddAnimation("saw", frames);
    mAnim->SetAnimation("saw");
    mAnim->SetAnimFPS(10.0f);

    mColliderComponent = new AABBColliderComponent(this,
                                                   10,
                                                   5,
                                                   30,
                                                   35,
                                                   ColliderLayer::EnemyBlocks,
                                                   true);
}

