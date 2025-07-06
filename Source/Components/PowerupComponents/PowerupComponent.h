//
// Created by lucas on 30/06/2025.
//

#pragma once
#include "../Component.h"

class PowerupComponent : public Component
{
public:
    explicit PowerupComponent(Actor *owner, float durationTime);

    virtual void Disable() = 0;

    virtual void ReleasePower();

    void Update(float deltaTime) override;

    bool IsUsable() const { return mIsUsable; }
    void SetUsable(const bool val) { mIsUsable = val; }

private:
    const float mOriginalDurationTime;
    float mDurationTime;
    bool mIsUsable;
};
