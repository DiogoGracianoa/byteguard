//
// Created by dti Digital on 29/06/2025.
//

#pragma once
#include "../UIElements/UIScreen.h"

class StoryScreen : public UIScreen
{
public:
    StoryScreen(class Game* game, const std::string& fontName);
    ~StoryScreen();
};
