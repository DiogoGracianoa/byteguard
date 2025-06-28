//
// Created by lucas on 28/06/2025.
//

#pragma once

#include "Actor.h"

class MissileSpawner : public Actor
{
public:
    explicit MissileSpawner(Game* game, float spawnDistance);

    void OnUpdate(float deltaTime) override;
private:
    float mSpawnDistance;
};
