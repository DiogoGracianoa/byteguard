//
// Created by Lucas N. Ferreira on 28/05/25.
//

#include "UIImage.h"
#include <SDL_image.h>

UIImage::UIImage(SDL_Renderer *renderer,
                 const std::string &imagePath,
                 const Vector2 &pos,
                 const Vector2 &size,
                 const Vector3 &color)
    : UIElement(pos, size, color),
      mTexture(nullptr),
      mRenderer(renderer),
      mVisible(true)
{
    SDL_Surface *surface = IMG_Load(imagePath.c_str());
    if (!surface)
    {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return;
    }

    mTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!mTexture) { SDL_Log("Failed to create texture: %s", SDL_GetError()); }
}

UIImage::~UIImage()
{
    if (mTexture)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }
}

void UIImage::Draw(SDL_Renderer *renderer, const Vector2 &screenPos)
{
    if (!mTexture || !mVisible) return;

    SDL_Rect destRect;
    destRect.x = static_cast<int>(screenPos.x + mPosition.x);
    destRect.y = static_cast<int>(screenPos.y + mPosition.y);
    destRect.w = static_cast<int>(mSize.x);
    destRect.h = static_cast<int>(mSize.y);

    SDL_RenderCopy(renderer, mTexture, nullptr, &destRect);
}
