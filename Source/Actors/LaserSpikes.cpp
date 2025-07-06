//
// Created by dti Digital on 05/07/2025.
//

#include "LaserSpikes.h"
LaserSpikes::LaserSpikes(Game *game, SDL_Renderer *renderer)
    : Actor(game),
      mRenderer(renderer)

{
    mAnim = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/LaserSpikes/laser_spikes.png",
        "../Assets/Sprites/LaserSpikes/laser_spikes.json"
    );
    std::vector<int> frames = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    mAnim->AddAnimation("laser", frames);
    mAnim->SetAnimation("laser");
    mAnim->SetAnimFPS(10.0f);

    mColliderComponent = new AABBColliderComponent(this,
                                                   5,
                                                   20,
                                                   30,
                                                   20,
                                                   ColliderLayer::EnemyBlocks,
                                                   true);
}