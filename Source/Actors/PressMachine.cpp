#include "PressMachine.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Game.h"


PressMachine::PressMachine(Game* game, SDL_Renderer* renderer)
    : Actor(game)
    , mRenderer(renderer)

{
    // Adiciona animação
    mAnim = new DrawAnimatedComponent(this, "../Assets/Animations/pressMachine.png", "../Assets/Animations/pressMachine.json");
    std::vector<int> frames = {0, 1, 2, 3, 4, 5, 6, 7};
    mAnim->AddAnimation("press", frames);
    mAnim->SetAnimation("press");
    mAnim->SetAnimFPS(10.0f);

    // Define posição inicial (você pode setar depois externamente também)
    //this->SetPosition(Vector2(360.0f, 320.0f));
    mColliderComponent = new AABBColliderComponent(this, 0, 0, 40, 40, ColliderLayer::EnemyBlocks, true);

    // Inicia com colisor em altura mínima
    if (mColliderComponent)
        mColliderComponent->SetDimensions(40.0f, 0.0f); // colisor será atualizado depois
}

void PressMachine::OnUpdate(float deltaTime)
{

    // Recupera o frame atual da animação
    int currentFrame = static_cast<int>(mAnim->GetAnimTimer()) % 8;

    // Altura baseada no frame (ajuste conforme necessário)
    // Exemplo com altura crescente linearmente de 8 até 64
    float heightByFrame[8] = {8, 16, 24, 32, 40, 48, 56, 64};
    float currentHeight = heightByFrame[currentFrame];

    // Atualiza o colisor conforme a parte da prensa visível
    if (mColliderComponent) {
        mColliderComponent->SetDimensions(32.0f, currentHeight);
        mColliderComponent->SetOffset(0.0f, currentHeight / 2.0f); // centraliza corretamente
    }
}