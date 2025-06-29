//
// Created by lucas on 28/06/2025.
//

#include "MissileSpawner.h"
#include "Missile.h"
#include "Player.h"
#include "../Game.h"

MissileSpawner::MissileSpawner(Game *game, const float spawnDistance)
    : Actor(game),
      mSpawnDistance(spawnDistance) {}

void MissileSpawner::OnUpdate(float deltaTime)
{
    if (abs(GetGame()->GetPlayer()->GetPosition().x - GetPosition().x) <
        mSpawnDistance)
    {
        const auto spawned = new Missile(GetGame());
        spawned->SetPosition(GetPosition());
        mState = ActorState::Destroy;
    }
}
