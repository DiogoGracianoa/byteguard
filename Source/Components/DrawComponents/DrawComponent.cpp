//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "DrawComponent.h"
#include "../../Actors/Actor.h"

DrawComponent::DrawComponent(class Actor *owner, const int drawOrder)
    : Component(owner),
      mIsVisible(true),
      mDrawOrder(drawOrder)
{
    //    mOwner->GetGame()->AddDrawable(this);
}

DrawComponent::~DrawComponent()
{
    //    mOwner->GetGame()->RemoveDrawable(this);
}


void DrawComponent::Draw(SDL_Renderer *renderer, const Vector3 &modColor) {}
