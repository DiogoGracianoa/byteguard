//
// Created by dti Digital on 29/06/2025.
//

#include "StoryScreen.h"
#include "../Game.h"

StoryScreen::StoryScreen(Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
{
    float windowWidth = static_cast<float>(mGame->GetWindowWidth());
    float windowHeight = static_cast<float>(mGame->GetWindowHeight());

    std::string storyText =
        "Em um futuro automatizado, uma falha crítica de sistema \n"
        "mergulhou a cidade no caos. Redes de energia, tráfego e comunicação \n"
        "foram corrompidas por uma IA hostil.\n\n"
        "Você é um robô de manutenção, a última esperança para restaurar o \n"
        "sistema central. Corra contra o tempo! Sua missão é desviar \n"
        "das armadilhas e perigos para trazer a cidade de volta à vida.";

    AddText(
        storyText,
        Vector2(80.0f, windowHeight * 0.1f),
        Vector2(windowWidth - 80, windowHeight * 0.6f),
        20,
        static_cast<int>(windowWidth * 0.8f)
    );

    AddButton(
        "Iniciar Missão",
        Vector2(windowWidth / 2.0f - 150.0f, windowHeight * 0.8f), // Posição mais para baixo
        Vector2(300.0f, 60.0f),
        [this]() {
            mGame->SetGameScene(Game::GameScene::TutorialLevel);
        }
    );
}

StoryScreen::~StoryScreen()
{
}