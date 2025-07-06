//
// Created by dti Digital on 05/07/2025.
//

#pragma once
#include "../UIElements/UIScreen.h"

class TutorialGuide : public UIScreen
{
public:
    TutorialGuide(class Game* game, std::string fontName = "../Assets/Fonts/Rajdhani-Bold.ttf");
    ~TutorialGuide();

    void HandleKeyPress(int key) override;

    void Draw(SDL_Renderer* renderer) override;
};

