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

    AddText(
        u8"MISSÃO CUMPRIDA!",
        Vector2(0.0f, windowHeight * 0.15f),
        Vector2(windowWidth, 50.0f),
        48,
        windowWidth
    );

    std::string winText = 
        "Parabéns! Você navegou pelos perigos e superou a IA hostil.\n"
        "O sistema central foi restaurado e a cidade, lentamente, \n"
        "voltará à vida.\n\n"
        "Graças à sua coragem e velocidade, a ordem foi restabelecida.\n"
        "A metrópole futurista está salva... por enquanto.";

    AddText(
        winText,
        Vector2(0.0f, windowHeight * 0.35f),
        Vector2(windowWidth, windowHeight * 0.4f),
        28,
        static_cast<int>(windowWidth * 0.8f)
    );


    // TODO: Adicionar botão de jogar novamente
    // AddButton(
    //     u8"Jogar Novamente",
    //     Vector2(windowWidth / 2.0f - 150.0f, windowHeight * 0.75f),
    //     Vector2(300.0f, 60.0f),
    //     [this]() {
    //         mGame->SetGameScene(GameScene::MainMenu); 
    //     }
    // );

    AddButton(
        "Sair do Jogo",
        Vector2(windowWidth / 2.0f - 150.0f, windowHeight * 0.75f + 70.0f),
        Vector2(300.0f, 60.0f),
        [this]() {
            mGame->Quit();
        }
    );
}

GameWinScreen::~GameWinScreen()
{
}