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

    const Vector2 imagePos(offsetTextAttempt_x, HUD_POS_Y);

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
}

HUD::~HUD() {}

void HUD::SetTime(const int time) {}

void HUD::SetLevelName(const std::string &levelName)
{
    //mLevelName->SetText(levelName);
}


void HUD::SetAttemptCount(const int count) const
{
    const std::string text = "Tentativa: " + std::to_string(count);
    mAttemptText->SetText(text);
}

