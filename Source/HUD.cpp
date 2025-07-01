//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "UIElements/UIText.h"

HUD::HUD(SDL_Renderer *renderer,
         class Game *game,
         const std::string &fontName)
    : UIScreen(game, fontName)
{
    constexpr float offsetTextAttempt_x = 15.0f;
    constexpr float HUD_CharWidth = 15.0f;
    constexpr float HUD_CharHeight = 15.0f;

    constexpr float imageWidth = (HUD_CharWidth * 15) + 30;
    constexpr float imageHeight = HUD_CharHeight * 2 + 15;

    const Vector2 imagePos(offsetTextAttempt_x, static_cast<float>(HUD_POS_Y));

    AddImage(
        renderer,
        "../Assets/Sprites/AttemptPlate.png",
        imagePos,
        Vector2(imageWidth, imageHeight)
    );


    constexpr float textWidth = HUD_CharWidth * 11;
    constexpr float textHeight = HUD_CharHeight;

    const Vector2 textPos(
        imagePos.x + (imageWidth - textWidth) / 2,
        (imagePos.y + (imageHeight - textHeight) / 2) + 5
    );

    mAttemptText = AddText(
        "Tentativa 1",
        textPos,
        Vector2(textWidth, textHeight),
        POINT_SIZE
    );

    constexpr float powerupContainerWidth = 100.0f;
    constexpr float powerupContainerHeight = 50.0f;

    const auto powerupContainerPos = textPos + Vector2(textWidth + 50, -15);

    AddImage(renderer,
             "../Assets/Sprites/Powerup_HUD.png",
             powerupContainerPos,
             Vector2(powerupContainerWidth, powerupContainerHeight)
    );

    constexpr float powerupSize = 30;

    const auto powerupPos = powerupContainerPos +
                            Vector2(powerupSize - 15,
                                    (powerupContainerHeight - powerupSize) / 2);

    AddText("Q",
            powerupContainerPos +
            Vector2(powerupContainerWidth / 2 + 5,
                    (powerupContainerHeight - powerupSize) / 2),
            Vector2(HUD_CharWidth * 2, HUD_CharHeight * 2));

    mTimePowerupImage = AddImage(renderer,
                                 "../Assets/Sprites/Collectibles/Time_Powerup.png",
                                 powerupPos,
                                 Vector2(powerupSize, powerupSize)
    );
    mTimePowerupImage->SetVisibility(false);
}

HUD::~HUD() = default;

void HUD::SetAttemptCount(const int count) const
{
    const std::string text = "Tentativa: " + std::to_string(count);
    mAttemptText->SetText(text);
}

void HUD::SetPowerupUsable(const bool isUsable) const
{
    isUsable
        ? mTimePowerupImage->SetVisibility(true)
        : mTimePowerupImage->SetVisibility(false);
}
