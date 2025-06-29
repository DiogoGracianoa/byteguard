// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "UIScreen.h"
#include "../Game.h"

UIScreen::UIScreen(Game *game, const std::string &fontName)
    : mGame(game),
      mPos(0.f, 0.f),
      mSize(0.f, 0.f),
      mState(UIState::Active),
      mSelectedButtonIndex(-1)
{
    mGame->PushUI(this);
    mFont = mGame->LoadFont(fontName);
}

UIScreen::~UIScreen()
{
    for (const auto &text: mTexts) { delete text; }
    mTexts.clear();

    for (const auto &button: mButtons) { delete button; }
    mButtons.clear();

    for (const auto &image: mImages) { delete image; }
    mImages.clear();
}

void UIScreen::Update(float deltaTime) {}

void UIScreen::Draw(SDL_Renderer *renderer)
{
    for (const auto &image: mImages) { image->Draw(renderer, mPos); }

    for (const auto &text: mTexts) { text->Draw(renderer, mPos); }

    for (const auto &button: mButtons) { button->Draw(renderer, mPos); }
}

void UIScreen::ProcessInput(const uint8_t *keys) {}

void UIScreen::HandleKeyPress(const int key)
{
    if (mButtons.empty()) return;

    if (key == SDLK_w)
    {
        if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>
            (mButtons.size()))
            mButtons[mSelectedButtonIndex]->SetHighlighted(
                false);

        mSelectedButtonIndex--;
        if (mSelectedButtonIndex <
            0)
            mSelectedButtonIndex = static_cast<int>(mButtons.size()) - 1;

        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
    }
    else if (key == SDLK_s)
    {
        if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>
            (mButtons.size()))
            mButtons[mSelectedButtonIndex]->SetHighlighted(
                false);

        mSelectedButtonIndex++;
        if (mSelectedButtonIndex >= static_cast<int>(mButtons.
                size()))
            mSelectedButtonIndex = 0;

        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
    }
    else if (key == SDLK_RETURN)
    {
        if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>
            (mButtons.size())) { mButtons[mSelectedButtonIndex]->OnClick(); }
    }
}

void UIScreen::Close() { mState = UIState::Closing; }

UIText *UIScreen::AddText(const std::string &name,
                          const Vector2 &pos,
                          const Vector2 &dims,
                          const int pointSize,
                          const int unsigned wrapLength)
{
    auto t = new UIText(name, mFont, pointSize, wrapLength, pos, dims,
                        Vector3(1.0f, 1.0f, 1.0f));
    mTexts.emplace_back(t);
    return t;
}

UIButton *UIScreen::AddButton(const std::string &name,
                              const Vector2 &pos,
                              const Vector2 &dims,
                              const std::function<void()> &onClick)
{
    const auto buttonColor = Vector3(30.0f, 77.0f, 93.0f);
    const auto textColor = Vector3(1.0f, 1.0f, 1.0f);
    const Vector2 textSize = dims * 0.6f;
    const Vector2 textPos = (dims * 0.5f) - (textSize * 0.5f);
    const unsigned wrapLength = dims.x * 2;

    auto b = new UIButton(
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

UIImage *UIScreen::AddImage(SDL_Renderer *renderer,
                            const std::string &imagePath,
                            const Vector2 &pos,
                            const Vector2 &dims,
                            const Vector3 &color)
{
    auto img = new UIImage(renderer, imagePath, pos, dims, color);

    mImages.emplace_back(img);

    return img;
}
