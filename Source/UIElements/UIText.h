//
// Created by Lucas N. Ferreira on 22/05/25.
//

#pragma once

#include <SDL.h>
#include <string>
#include "UIElement.h"
#include "../Math.h"

class UIText final : public UIElement
{
public:
    UIText(const std::string &text,
           class UIFont *font,
           int pointSize = 40,
           unsigned wrapLength = 1024,
           const Vector2 &pos = Vector2::Zero,
           const Vector2 &size = Vector2(100.f, 20.0f),
           const Vector3 &color = Color::White);

    ~UIText() override;

    void SetText(const std::string &name);

    void Draw(SDL_Renderer *renderer, const Vector2 &screenPos) override;

protected:
    std::string mText;
    class UIFont *mFont;
    SDL_Texture *mTextTexture;

    unsigned int mPointSize;
    unsigned int mWrapLength;
};
