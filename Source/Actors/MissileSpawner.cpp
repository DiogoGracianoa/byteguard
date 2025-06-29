//
// Created by lucas on 28/06/2025.
//

#include "MissileSpawner.h"
#include "../Game.h"
#include "Mario.h"
#include "Missile.h"

MissileSpawner::MissileSpawner(Game* game, float spawnDistance)
        :Actor(game)
        ,mSpawnDistance(spawnDistance)
{

}

void MissileSpawner::OnUpdate(float deltaTime)
{
    if (abs(GetGame()->GetMario()->GetPosition().x - GetPosition().x) < mSpawnDistance)
    {
        auto spawned = new Missile(GetGame());
        spawned->SetPosition(GetPosition());
        mState = ActorState::Destroy;
    }
}