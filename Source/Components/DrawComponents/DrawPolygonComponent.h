//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include <vector>
#include "DrawComponent.h"

class DrawPolygonComponent final : public DrawComponent
{
public:
    // (Lower draw order corresponds with further back)
    DrawPolygonComponent(class Actor *owner,
                         const std::vector<Vector2> &vertices,
                         int drawOrder = 100);

    void Draw(SDL_Renderer *renderer,
              const Vector3 &modColor = Color::White) override;

    std::vector<Vector2> &GetVertices() { return mVertices; }

protected:
    int mDrawOrder;
    std::vector<Vector2> mVertices;
};
