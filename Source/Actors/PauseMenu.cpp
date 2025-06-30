//
// Created by dti Digital on 29/06/2025.
//

#include "PauseMenu.h"
#include "../Game.h"

PauseMenu::PauseMenu(Game* game, std::string fontName)
    : UIScreen(game, fontName)
{
    float windowWidth = static_cast<float>(mGame->GetWindowWidth());
    float windowHeight = static_cast<float>(mGame->GetWindowHeight());

    AddText(
        "JOGO PAUSADO",
        Vector2(windowWidth/2 - 150, windowHeight * 0.25f),
        Vector2(300, 50.0f),
        40,
        windowWidth
    );

    Vector2 buttonDims(200.0f, 50.0f);
    Vector2 buttonPos(
        (windowWidth - buttonDims.x) / 2.0f,
        windowHeight * 0.5f
    );

    AddButton(
        "Continuar",
        buttonPos,
        buttonDims,
        [this]() {
            Close();
            mGame->TogglePause();
        }
    );
}

PauseMenu::~PauseMenu()
{
}

void PauseMenu::HandleKeyPress(int key)
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

void PauseMenu::Draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_Rect screenRect = { 0, 0, mGame->GetWindowWidth(), mGame->GetWindowHeight() };
    SDL_RenderFillRect(renderer, &screenRect);
    UIScreen::Draw(renderer);
}