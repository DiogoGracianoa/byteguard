//
// Created by dti Digital on 05/07/2025.
//

#include "WallBlades.h"

WallBlades::WallBlades(Game *game, SDL_Renderer *renderer)
    : Actor(game),
      mRenderer(renderer)

{
    mAnim = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/WallBlades/wall_blades.png",
        "../Assets/Sprites/WallBlades/wall_blades.json"
    );
    std::vector<int> frames = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28};
    mAnim->AddAnimation("wall_blades", frames);
    mAnim->SetAnimation("wall_blades");
    mAnim->SetAnimFPS(10.0f);

    mColliderComponent = new AABBColliderComponent(this,
                                                   5,
                                                   0,
                                                   30,
                                                   40,
                                                   ColliderLayer::EnemyBlocks,
                                                   true);
}