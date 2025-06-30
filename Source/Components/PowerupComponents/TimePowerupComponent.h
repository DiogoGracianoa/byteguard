//
// Created by Diogo Graciano on 14/06/2025.
//

#pragma once

#include <string>
#include "PowerupComponent.h"
#include "../../Actors/Actor.h"

class TimePowerupComponent final : public PowerupComponent
{
public:
    explicit TimePowerupComponent(Actor *owner,
                                  float durationTime,
                                  float slowingFactor);

    void ReleasePower() override;

    void Disable() override;

private:
    float mSlowingFactor;
};
