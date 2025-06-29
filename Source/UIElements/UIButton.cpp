//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIButton.h"

UIButton::UIButton(const std::string &text,
                   class UIFont *font,
                   const std::function<void()> &onClick,
                   const Vector2 &pos,
                   const Vector2 &size,
                   const Vector3 &color,
                   const int pointSize,
                   const unsigned wrapLength,
                   const Vector2 &textPos,
                   const Vector2 &textSize,
                   const Vector3 &textColor)
    : UIElement(pos, size, color),
      mOnClick(onClick),
      mText(text, font, pointSize, wrapLength, textPos, textSize, textColor),
      mHighlighted(false) {}

UIButton::~UIButton() {}

void UIButton::Draw(SDL_Renderer *renderer, const Vector2 &screenPos)
{
    SDL_Rect buttonRect;
    buttonRect.x = static_cast<int>(mPosition.x + screenPos.x);
    buttonRect.y = static_cast<int>(mPosition.y + screenPos.y);
    buttonRect.w = static_cast<int>(mSize.x);
    buttonRect.h = static_cast<int>(mSize.y);

    if (mHighlighted)
    {
        const auto color = this->mColor;
        SDL_SetRenderDrawColor(renderer, color.x, color.y, color.z, 255);
        SDL_RenderFillRect(renderer, &buttonRect);
    }

    mText.Draw(renderer, mPosition + screenPos);
}

void UIButton::OnClick() const { if (mOnClick) { mOnClick(); } }
