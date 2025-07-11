#include "UIFont.h"
#include <SDL_image.h>
#include <vector>

UIFont::UIFont(SDL_Renderer *renderer)
	: mRenderer(renderer) {}

UIFont::~UIFont() {}

bool UIFont::Load(const std::string &fileName)
{
	// We support these font sizes
	std::vector<int> fontSizes = {
			8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
			34, 36, 38, 40, 42, 44, 46, 48, 52, 56, 60, 64, 68, 72
			};

	for (int size: fontSizes)
	{
		TTF_Font *font = TTF_OpenFont(fileName.c_str(), size);
		if (!font)
		{
			SDL_Log("Failed to load font size %d: %s", size, TTF_GetError());
			return false;
		}
		mFontData[size] = font;
	}

	return true;
}

void UIFont::Unload()
{
	for (const auto &[_, second]: mFontData) { TTF_CloseFont(second); }
	mFontData.clear();
}

SDL_Texture *UIFont::RenderText(const std::string &text,
                                const Vector3 &color /*= Color::White*/,
                                const int pointSize /*= 24*/,
                                const unsigned wrapLength /*= 900*/)
{
	if (!mRenderer)
	{
		SDL_Log("Renderer is null. Can't Render Text!");
		return nullptr;
	}

	// Convert to SDL_Color
	SDL_Color sdlColor;

	// Swap red and blue so we get RGBA instead of BGRA
	sdlColor.b = static_cast<Uint8>(color.x * 255);
	sdlColor.g = static_cast<Uint8>(color.y * 255);
	sdlColor.r = static_cast<Uint8>(color.z * 255);
	sdlColor.a = 255;

	const auto iter = mFontData.find(pointSize);
	if (iter == mFontData.end())
	{
		SDL_Log("Unsupported font size: %d", pointSize);
		return nullptr;
	}

	TTF_Font *font = iter->second;
	SDL_Surface *surf = TTF_RenderUTF8_Blended_Wrapped(
		font,
		text.c_str(),
		sdlColor,
		wrapLength
	);

	if (!surf)
	{
		SDL_Log("Failed to render text surface: %s", TTF_GetError());
		return nullptr;
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, surf);
	SDL_FreeSurface(surf);

	if (!texture)
	{
		SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
		return nullptr;
	}

	return texture;
}
