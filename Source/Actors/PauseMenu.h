//
// Created by dti Digital on 29/06/2025.
//

#pragma once
#include "../UIElements/UIScreen.h"

class PauseMenu : public UIScreen
{
public:
    PauseMenu(class Game* game, std::string fontName = "../Assets/Fonts/Rajdhani-Bold.ttf");
    ~PauseMenu();

    void HandleKeyPress(int key) override;

    void Draw(SDL_Renderer* renderer) override;
};
