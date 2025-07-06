//
// Created by dti Digital on 29/06/2025.
//

#include "GameWinScreen.h"
#include "../Game.h"

GameWinScreen::GameWinScreen(Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
{
    float windowWidth = static_cast<float>(mGame->GetWindowWidth());
    float windowHeight = static_cast<float>(mGame->GetWindowHeight());

    auto titlePointSize = 40;
    auto titleWidth = 15 * titlePointSize;

    AddText(
        u8"MISSÃO CUMPRIDA!",
        Vector2(windowWidth/2 - (titleWidth/2), windowHeight * 0.10f),
        Vector2(titleWidth, 50.0f),
        48,
        windowWidth
    );

    std::string winText = 
        "Parabéns! Você navegou pelos perigos e superou a IA hostil.\n"
        "O sistema central foi restaurado e a cidade lentamente voltará à vida, \n\n "
        "Graças à sua coragem e velocidade, a ordem foi restabelecida.\n"
        "A metrópole futurista está salva... por enquanto.";

    AddText(
        winText,
        Vector2(130.0f, windowHeight * 0.25f),
        Vector2(windowWidth - 160, windowHeight * 0.4f),
        20,
        static_cast<int>(windowWidth * 0.8f)
    );

    AddButton(
        "Sair do Jogo",
        Vector2(windowWidth / 2.0f - 150.0f, windowHeight * 0.75f),
        Vector2(300.0f, 60.0f),
        [this]() {
            mGame->Quit();
        }
    );
}

GameWinScreen::~GameWinScreen()
{
}