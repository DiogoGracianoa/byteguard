//
// Created by lucas on 28/06/2025.
//

#pragma once

#include "Actor.h"

class MissileSpawner final : public Actor
{
public:
    explicit MissileSpawner(Game *game,
                            float spawnDistance,
                            float SForceFactor = 5.0f);

    void OnUpdate(float deltaTime) override;

private:
    float mSpawnDistance;
    float mSForceFactor;
};
