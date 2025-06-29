//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIText.h"
#include "UIFont.h"

UIText::UIText(const std::string &text,
               class UIFont *font,
               const int pointSize,
               const unsigned wrapLength,
               const Vector2 &pos,
               const Vector2 &size,
               const Vector3 &color)
    : UIElement(pos, size, color),
      mFont(font),
      mTextTexture(nullptr),
      mPointSize(pointSize),
      mWrapLength(wrapLength) { SetText(text); }

UIText::~UIText() {}

void UIText::SetText(const std::string &name)
{
    if (mTextTexture)
    {
        SDL_DestroyTexture(mTextTexture);
        mTextTexture = nullptr;
    }

    mTextTexture = mFont->RenderText(name, mColor, mPointSize, mWrapLength);

    if (!mTextTexture)
    {
        SDL_Log("UIText: Failed to render text texture.");
        return;
    }

    mText = name;
}

void UIText::Draw(SDL_Renderer *renderer, const Vector2 &screenPos)
{
    if (!mTextTexture) { return; }
    SDL_Rect titleQuad;
    titleQuad.x = static_cast<int>(mPosition.x + screenPos.x);
    titleQuad.y = static_cast<int>(mPosition.y + screenPos.y);
    titleQuad.w = static_cast<int>(mSize.x);
    titleQuad.h = static_cast<int>(mSize.y);

    SDL_RenderCopyEx(renderer,
                     mTextTexture,
                     nullptr,
                     &titleQuad,
                     0.0,
                     nullptr,
                     SDL_FLIP_NONE);
}
