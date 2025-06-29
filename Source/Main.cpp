// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#define SDL_MAIN_HANDLED
#include "Game.h"

//Screen dimension constants
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 560;

int main(int argc, char **argv)
{
    auto game = Game(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (game.Initialize()) { game.RunLoop(); }
    game.Shutdown();
    return 0;
}
