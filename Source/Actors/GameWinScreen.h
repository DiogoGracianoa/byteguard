//
// Created by dti Digital on 29/06/2025.
//

#pragma once
#include "../UIElements/UIScreen.h"

class GameWinScreen : public UIScreen
{
public:
    GameWinScreen(class Game* game, const std::string& fontName = "../Assets/Fonts/Rajdhani-Bold.ttf"        );
    ~GameWinScreen();
};
