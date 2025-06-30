//
// Created by dti Digital on 29/06/2025.
//

#pragma once
#include "../UIElements/UIScreen.h"

class GamewinScreen : public UIScreen
{
public:
    GamewinScreen(class Game* game, const std::string& fontName);
    ~GamewinScreen();
};
