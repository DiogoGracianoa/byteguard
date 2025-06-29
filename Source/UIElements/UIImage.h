//
// Created by Lucas N. Ferreira on 28/05/25.
//

#pragma once

#include <string>
#include "UIElement.h"

class UIImage final : public UIElement
{
public:
    UIImage(SDL_Renderer *renderer,
            const std::string &imagePath,
            const Vector2 &pos = Vector2::Zero,
            const Vector2 &size = Vector2(100.f, 100.f),
            const Vector3 &color = Color::White);

    ~UIImage() override;

    void Draw(SDL_Renderer *renderer, const Vector2 &screenPos) override;

private:
    SDL_Texture *mTexture; // Texture for the image
    SDL_Renderer *mRenderer;
};
