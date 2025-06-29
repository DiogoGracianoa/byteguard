// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "UIScreen.h"
#include "../Game.h"
#include "UIFont.h"

UIScreen::UIScreen(Game* game, const std::string& fontName)
	:mGame(game)
	,mPos(0.f, 0.f)
	,mSize(0.f, 0.f)
	,mState(UIState::Active)
    ,mSelectedButtonIndex(-1)
{
    // --------------
    // TODO - PARTE 1-1
    // --------------

    // TODO 1.: Adicione a UIScreen à lista de telas de UI do jogo (mGame) com PushUI(this).
    mGame->PushUI(this);

    // TODO 2.: Utilize o método LoadFont do jogo (mGame) para carregar a fonte padrão,
    //  passada como parâmetro (fontName).
    mFont = mGame->LoadFont(fontName);
}

UIScreen::~UIScreen()
{
    // --------------
    // TODO - PARTE 1-1
    // --------------

    // TODO 1.: Percorra a listas de textos (mTexts) e delete cada UIText. Limpe a lista ao final do laço.
    for (auto text : mTexts)
    {
        delete text;
    }
    mTexts.clear();

    // --------------
    // TODO - PARTE 1-2
    // --------------

    // TODO 1.: Percorra a lista de botões (mButtons) e delete cada UIButton. Limpe a lista ao final do laço.
    for (auto button : mButtons)
    {
        delete button;
    }
    mButtons.clear();

    // --------------
    // TODO - PARTE 1-3
    // --------------

    // TODO 1.: Percorra a lista de imagens (mImages) e delete cada UIImage. Limpe a lista ao final do laço.
    for (auto image : mImages)
    {
        delete image;
    }
    mImages.clear();
}

void UIScreen::Update(float deltaTime)
{
	
}

void UIScreen::Draw(SDL_Renderer *renderer)
{
    for (auto image : mImages)
    {
        image->Draw(renderer, mPos);
    }

    for (auto text : mTexts)
    {
        text->Draw(renderer, mPos);
    }

    for (auto button : mButtons)
    {
        button->Draw(renderer, mPos);
    }
}

void UIScreen::ProcessInput(const uint8_t* keys)
{

}

void UIScreen::HandleKeyPress(int key)
{
    if (mButtons.empty())
        return;

    if (key == SDLK_w)
    {
        if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size()))
            mButtons[mSelectedButtonIndex]->SetHighlighted(false);

        mSelectedButtonIndex--;
        if (mSelectedButtonIndex < 0)
            mSelectedButtonIndex = static_cast<int>(mButtons.size()) - 1;

        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
    }
    else if (key == SDLK_s)
    {
        if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size()))
            mButtons[mSelectedButtonIndex]->SetHighlighted(false);

        mSelectedButtonIndex++;
        if (mSelectedButtonIndex >= static_cast<int>(mButtons.size()))
            mSelectedButtonIndex = 0;

        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
    }
    else if (key == SDLK_RETURN)
    {
        if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size()))
        {
            mButtons[mSelectedButtonIndex]->OnClick();
        }
    }
}

void UIScreen::Close()
{
	mState = UIState::Closing;
}

UIText* UIScreen::AddText(const std::string &name, const Vector2 &pos, const Vector2 &dims, const int pointSize, const int unsigned wrapLength)
{
    UIText* t = new UIText(name, mFont, pointSize, wrapLength, pos, dims, Vector3(1.0f, 1.0f, 1.0f));
    mTexts.emplace_back(t);
    return t;
}

UIButton* UIScreen::AddButton(const std::string& name, const Vector2 &pos, const Vector2& dims, std::function<void()> onClick)
{
    Vector3 buttonColor = Vector3(30.0f, 77.0f, 93.0f);
    Vector3 textColor = Vector3(1.0f, 1.0f, 1.0f);
    Vector2 textSize = dims * 0.6f;
    Vector2 textPos = (dims * 0.5f) - (textSize * 0.5f);
    unsigned wrapLength = dims.x * 2;

    UIButton* b = new UIButton(
        name,
        mFont,
        onClick,
        pos,
        dims,
        buttonColor,
        32,
        wrapLength,
        textPos,
        textSize,
        textColor
    );

    mButtons.emplace_back(b);

    if (mButtons.size() == 1)
    {
        mSelectedButtonIndex = 0;
        b->SetHighlighted(true);
    }

    return b;
}

UIImage* UIScreen::AddImage(SDL_Renderer* renderer, const std::string &imagePath, const Vector2 &pos, const Vector2 &dims, const Vector3 &color)
{
    UIImage* img = new UIImage(renderer, imagePath, pos, dims, color);

    mImages.emplace_back(img);

    return img;
}