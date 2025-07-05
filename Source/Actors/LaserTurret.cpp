//
// Created by dti Digital on 05/07/2025.
//

#include "LaserTurret.h"

LaserTurret::LaserTurret(Game *game, SDL_Renderer *renderer)
    : Actor(game),
      mRenderer(renderer)

{
    mAnim = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/LaserTurret/laser_turret.png",
        "../Assets/Sprites/LaserTurret/laser_turret.json"
    );
    std::vector<int> frames = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    mAnim->AddAnimation("laser_turret", frames);
    mAnim->SetAnimation("laser_turret");
    mAnim->SetAnimFPS(10.0f);

    mColliderComponent = new AABBColliderComponent(this,
                                                   0,
                                                   5,
                                                   40,
                                                   35,
                                                   ColliderLayer::EnemyBlocks,
                                                   true);
}