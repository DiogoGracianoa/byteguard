//
// Created by Diogo Graciano on 14/06/2025.
//

#include "TimePowerupComponent.h"
#include "../../Game.h"
#include "../DrawComponents/DrawSpriteComponent.h"

TimePowerupComponent::TimePowerupComponent(Actor *owner,
                                           const float durationTime,
                                           const float slowingFactor)
    : PowerupComponent(owner, durationTime),
      mSlowingFactor(slowingFactor) {}

void TimePowerupComponent::ReleasePower()
{
    PowerupComponent::ReleasePower();
    mOwner->GetGame()->SetSlowingFactor(2.0f);
}

void TimePowerupComponent::Disable()
{
    mOwner->GetGame()->ResetSlowingFactor();
}

