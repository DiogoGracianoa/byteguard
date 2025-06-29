#include "PressMachine.h"
#include "../Game.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"


PressMachine::PressMachine(Game *game, SDL_Renderer *renderer)
    : Actor(game),
      mRenderer(renderer)

{
    mAnim = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/PressMachine/pressMachine.png",
        "../Assets/Sprites/PressMachine/pressMachine.json"
    );
    std::vector<int> frames = {0, 1, 2, 3, 4, 5, 6, 7};
    mAnim->AddAnimation("press", frames);
    mAnim->SetAnimation("press");
    mAnim->SetAnimFPS(10.0f);

    mColliderComponent = new AABBColliderComponent(this,
                                                   0,
                                                   0,
                                                   40,
                                                   80,
                                                   ColliderLayer::EnemyBlocks,
                                                   true);

    mColliderComponent->SetDimensions(40.0f, 0.0f);
    // colisor será atualizado depois
}

void PressMachine::OnUpdate(float deltaTime)
{
    const int currentFrame = static_cast<int>(mAnim->GetAnimTimer()) % 8;

    const float heightByFrame[8] = {40, 60, 75, 75, 75, 60, 50, 40};
    const float currentHeight = heightByFrame[currentFrame];

    if (mColliderComponent)
    {
        mColliderComponent->SetDimensions(40.0f, currentHeight);
        //mColliderComponent->SetOffset(0.0f, currentHeight / 2.0f); // centraliza corretamente
    }
}
