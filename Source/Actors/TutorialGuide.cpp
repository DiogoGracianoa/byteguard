//
// Created by dti Digital on 05/07/2025.
//

#include "TutorialGuide.h"
#include "../Game.h"

TutorialGuide::TutorialGuide(Game* game, std::string fontName)
    : UIScreen(game, fontName)
{
    float windowWidth = static_cast<float>(mGame->GetWindowWidth());
    float windowHeight = static_cast<float>(mGame->GetWindowHeight());

    AddText(
        "Tutorial",
        Vector2(windowWidth/2 - 150, windowHeight * 0.10f),
        Vector2(300, 50.0f),
        34,
        windowWidth
    );

    std::string storyText =
        "Você coletou um POWER-UP de TEMPO.\n"
        "Aperte a tecla Q para usá-lo quando\n"
        "mais precisar.      \n\n"
        "Além do power-up, você pode frear\n"
        "com a tecla A, acelerar com a\n"
        "tecla D e pular com a tecla SPACE.\n\n"
        "Mas cuidado, não freie demais, \n"
        "há um míssil te seguindo.\n"
        "Boa sorte!";

    AddText(
        storyText,
        Vector2(250.0f, windowHeight * 0.20f),
        Vector2(windowWidth - 80, windowHeight * 0.6f),
        20,
        static_cast<int>(windowWidth * 0.8f)
    );


    AddButton(
        "Continuar",
        Vector2(windowWidth / 2.0f - 150.0f, windowHeight * 0.85f), // Posição mais para baixo
        Vector2(300.0f, 60.0f),
        [this]() {
            Close();
            mGame->ToggleTutorial();
        }
    );

}

TutorialGuide::~TutorialGuide()
{
}

void TutorialGuide::HandleKeyPress(int key)
{
    if (key == SDLK_RETURN)
    {
        if (mSelectedButtonIndex >= 0)
        {
            mButtons[mSelectedButtonIndex]->OnClick();
        }
    }
    else
    {
        UIScreen::HandleKeyPress(key);
    }
}

void TutorialGuide::Draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_Rect screenRect = { 0, 0, mGame->GetWindowWidth(), mGame->GetWindowHeight() };
    SDL_RenderFillRect(renderer, &screenRect);
    UIScreen::Draw(renderer);
}