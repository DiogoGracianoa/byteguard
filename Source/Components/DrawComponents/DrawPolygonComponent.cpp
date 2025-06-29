//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "DrawPolygonComponent.h"
#include "../../Game.h"
#include "../../Actors/Actor.h"

DrawPolygonComponent::DrawPolygonComponent(class Actor *owner,
                                           const std::vector<Vector2> &vertices,
                                           const int drawOrder)
    : DrawComponent(owner),
      mDrawOrder(drawOrder),
      mVertices(vertices) {}

void DrawPolygonComponent::Draw(SDL_Renderer *renderer, const Vector3 &modColor)
{
    // Set draw color to green
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    Vector2 pos = mOwner->GetPosition();
    Vector2 cameraPos = mOwner->GetGame()->GetCameraPos();

    // Render vertices as lines
    for (int i = 0; i < mVertices.size() - 1; i++)
    {
        SDL_RenderDrawLine(renderer, pos.x + mVertices[i].x - cameraPos.x,
                           pos.y + mVertices[i].y - cameraPos.y,
                           pos.x + mVertices[i + 1].x - cameraPos.x,
                           pos.y + mVertices[i + 1].y - cameraPos.y);
    }

    // Close geometry
    SDL_RenderDrawLine(renderer,
                       pos.x + mVertices[mVertices.size() - 1].x - cameraPos.x,
                       pos.y + mVertices[mVertices.size() - 1].y - cameraPos.y,
                       pos.x + mVertices[0].x - cameraPos.x,
                       pos.y + mVertices[0].y - cameraPos.y);
}
