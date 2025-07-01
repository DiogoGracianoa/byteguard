//
// Created by Lucas N. Ferreira on 08/12/23.
//

#pragma once

#include <string>

#include "UIElements/UIScreen.h"

class HUD final : public UIScreen
{
public:
    const int POINT_SIZE = 48;
    const int WORD_HEIGHT = 20.0f;
    const int WORD_OFFSET = 25.0f;
    const int CHAR_WIDTH = 20.0f;
    const int HUD_POS_Y = 10.0f;

    HUD(SDL_Renderer *renderer, class Game *game, const std::string &fontName);

    ~HUD() override;

    void SetAttemptCount(int count) const;

    void SetPowerupUsable(bool isUsable) const;

    void CreateAttemptTextWithOutline(const std::string &text,
                                      const Vector2 &basePos,
                                      int pointSize);

private:
    // HUD elements
    UIText *mAttemptText;
    UIImage *mTimePowerupImage;
};
