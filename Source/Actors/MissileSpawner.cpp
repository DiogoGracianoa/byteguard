//
// Created by lucas on 28/06/2025.
//

#include "MissileSpawner.h"

#include "Block.h"
#include "Missile.h"
#include "Player.h"
#include "../Game.h"

MissileSpawner::MissileSpawner(Game *game,
                               const float spawnDistance,
                               const float SForceFactor)
    : Actor(game),
      mSpawnDistance(spawnDistance),
      mSForceFactor(SForceFactor)
{
    new DrawSpriteComponent(this,
                            "../Assets/Sprites/Missile/Missile_Launcher.png",
                            Game::TILE_SIZE - 5,
                            Game::TILE_SIZE / 1.3,
                            99);
}

void MissileSpawner::OnUpdate(float deltaTime)
{
    const auto seekVector = mGame->GetPlayer()->GetPosition() - mPosition;
    mRotation = Math::Atan2(seekVector.y, seekVector.x);
    if (abs(mGame->GetPlayer()->GetPosition().x - GetPosition().x) <
        mSpawnDistance)
    {
        const auto spawned = new Missile(GetGame(), 330, mSForceFactor);
        spawned->SetPosition(GetPosition() + Vector2(5, 0));
        mState = ActorState::Destroy;
    }
}
