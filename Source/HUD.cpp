//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "UIElements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 1.: Adicione um texto com a string "Time" no canto superior direito da tela, como no jogo orginal. Note que
    //  a classe HUD tem constantes WORD_HEIGHT, WORD_OFFSET, CHAR_WIDTH, POINT_SIZE e HUD_POS_Y que podem ser usadas
    //  para posicionar e definir o tamanho do texto.
    const float screenWidth = static_cast<float>(game->GetWindowWidth());
    AddText(
        "Time",
        Vector2(screenWidth - WORD_OFFSET - CHAR_WIDTH * 4, HUD_POS_Y),
        Vector2(CHAR_WIDTH * 4, WORD_HEIGHT),
        POINT_SIZE
        );

    // TODO 2.: Adicione um texto com a string "400" (400 segundos) logo abaixo do texto "Time".
    //  Guarde o ponteiro do texto em um membro chamado mTimeText.
    mTimeText = AddText(
        "400",
        Vector2(screenWidth - WORD_OFFSET - CHAR_WIDTH * 3, HUD_POS_Y + WORD_HEIGHT),
        Vector2(CHAR_WIDTH * 3, WORD_HEIGHT),
        POINT_SIZE
        );

    // TODO 3.: Adicione um texto com a string "World" à esquerda do texto "Time", como no jogo original.
    AddText(
        "World",
        Vector2(screenWidth * 0.5f - CHAR_WIDTH * 2, HUD_POS_Y),
        Vector2(CHAR_WIDTH*5, WORD_HEIGHT),
        POINT_SIZE
        );

    // TODO 4.: Adicione um texto com a string "1-1" logo abaixo do texto "World".
    // Não esquecer -> Dica da colega Luiza no discord: falta uma instrução pra armazenar o ponteiro em mLevelName. Senão dá um segfault na função SetLevelName
    mLevelName = AddText(
        "1-1",
        Vector2(screenWidth * 0.5f - CHAR_WIDTH, HUD_POS_Y + WORD_HEIGHT),
        Vector2(CHAR_WIDTH*3, WORD_HEIGHT),
        POINT_SIZE
        );

    // TODO 5.: Adicione um texto com a string "Mario" no canto superior esquerdo da tela, como no jogo original.
    AddText(
        "Mario",
        Vector2(WORD_OFFSET, HUD_POS_Y),
        Vector2(CHAR_WIDTH*5, WORD_HEIGHT),
        POINT_SIZE
        );

    // TODO 6.: Adicione um texto com a string "000000" logo abaixo do texto "Mario".
    // aqui vai entrar o contador de moeda
    mCoinsCountText = AddText(
        "000000",
        Vector2(WORD_OFFSET, HUD_POS_Y + WORD_HEIGHT),
        Vector2(CHAR_WIDTH*6, WORD_HEIGHT),
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
    std::string timeStr = std::to_string(time);
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
    }
}

void HUD::SetLevelName(const std::string &levelName)
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 1.: Utilize o método SetText() do mLevelName para atualizar o texto com o nome do nível.
    mLevelName->SetText(levelName);
}

void HUD::SetCoinsCount(int coinsCount)
{
    std::string coinsCountStr = std::to_string(coinsCount);
    int digits = static_cast<int>(coinsCountStr.size());

    while (digits < 6) {
        coinsCountStr = "0" + coinsCountStr;
        digits++;
    }

    mCoinsCountText->SetText(coinsCountStr);
}