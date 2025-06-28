//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "UIElements/UIText.h"

HUD::HUD(SDL_Renderer *renderer, class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    const float screenWidth = static_cast<float>(game->GetWindowWidth());
    /*AddText(
        "Time",
        Vector2(screenWidth - WORD_OFFSET - CHAR_WIDTH * 4, HUD_POS_Y),
        Vector2(CHAR_WIDTH * 4, WORD_HEIGHT),
        POINT_SIZE
        );*/

    /*mTimeText = AddText(
        "400",
        Vector2(screenWidth - WORD_OFFSET - CHAR_WIDTH * 3, HUD_POS_Y + WORD_HEIGHT),
        Vector2(CHAR_WIDTH * 3, WORD_HEIGHT),
        POINT_SIZE
        );*/

    auto offsetTextAttempt_x = 15.0f;
    auto HUD_CharWidth = 15.0f;
    auto HUD_CharHeight = 15.0f;

    float imageWidth = (HUD_CharWidth * 15) + 30 ;
    float imageHeight = HUD_CharHeight * 2 + 15;

    Vector2 imagePos(offsetTextAttempt_x, HUD_POS_Y);

    AddImage(
        renderer,
        "../Assets/Sprites/AttemptPlate.png",
        imagePos,
        Vector2(imageWidth, imageHeight)
    );


    float textWidth = HUD_CharWidth * 11;
    float textHeight = HUD_CharHeight;

    Vector2 textPos(
        imagePos.x + (imageWidth - textWidth) / 2,
        (imagePos.y + (imageHeight - textHeight) / 2) + 5
    );

    mAttemptText = AddText(
        "Tentativa 1",
        textPos,
        Vector2(textWidth, textHeight),
        POINT_SIZE
    );
}

HUD::~HUD()
{

}

void HUD::SetTime(int time)
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 1.: Utilize o método SetText() do mTimeText para atualizar o texto com o tempo restante. Lembre-se que
    //  o tempo é um inteiro que representa os segundos restantes, e deve ser convertido para string.
    /*std::string timeStr = std::to_string(time);
    mTimeText->SetText(timeStr);

    // TODO 2.: A posição e o tamanho do texto irão mudar dependendo do número de dígitos na variável time.
    //  Ajuste a posição e o tamanho do mTimeText de acordo com o número de dígitos, de tal forma que
    //  o texto fique alinhado à direita com o texto "Time" e o tamanho do texto seja proporcional ao número de dígitos.
    int digits = static_cast<int>(timeStr.size());

    float newWidth = CHAR_WIDTH * digits;
    float height = WORD_HEIGHT;

    Vector2 basePos = mTimeText->GetPosition();
    Vector2 baseSize = mTimeText->GetSize();

    auto oldWidth = baseSize.x;
    if (oldWidth > newWidth) {
        float newX = basePos.x + CHAR_WIDTH;
        mTimeText->SetPosition(Vector2(newX, basePos.y));
        mTimeText->SetSize(Vector2(newWidth, height));
    }*/
}

void HUD::SetLevelName(const std::string &levelName)
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 1.: Utilize o método SetText() do mLevelName para atualizar o texto com o nome do nível.
    //mLevelName->SetText(levelName);
}

void HUD::SetCoinsCount(int coinsCount)
{
    std::string coinsCountStr = std::to_string(coinsCount);
    int digits = static_cast<int>(coinsCountStr.size());

    while (digits < 6) {
        coinsCountStr = "0" + coinsCountStr;
        digits++;
    }

    //mCoinsCountText->SetText(coinsCountStr);
}

void HUD::SetAttemptCount(int count)
{
    std::string texto = "Tentativa: " + std::to_string(count);
    mAttemptText->SetText(texto);
}

