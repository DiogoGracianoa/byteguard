//
// Created by lucas on 30/06/2025.
//

#include "PowerupComponent.h"

PowerupComponent::PowerupComponent(Actor *owner, const float durationTime)
    : Component(owner),
      mDurationTime(durationTime),
      mIsUsable(false) { mIsEnabled = false; }

void PowerupComponent::ReleasePower()
{
    if (mIsUsable)
    {
        mIsEnabled = true;
        mIsUsable = false;
    }
}


void PowerupComponent::Update(const float deltaTime)
{
    if (mIsEnabled)
    {
        mDurationTime -= deltaTime;
        if (mDurationTime <= 0)
        {
            mIsEnabled = false;
            Disable();
        }
    }
}
