cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "byteguard")
project(${PROJECT_NAME})

find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_executable(${PROJECT_NAME}
        Source/Main.cpp
        Source/Math.cpp
        Source/Random.cpp
        Source/Actors/Actor.cpp
        Source/Components/Component.cpp
        Source/Game.cpp
        Source/Components/DrawComponents/DrawComponent.cpp
        Source/Components/DrawComponents/DrawComponent.h
        Source/Components/RigidBodyComponent.cpp
        Source/Components/RigidBodyComponent.h
        Source/Json.h
        Source/CSV.h
        Source/Components/DrawComponents/DrawSpriteComponent.cpp
        Source/Components/DrawComponents/DrawSpriteComponent.h
        Source/Components/DrawComponents/DrawPolygonComponent.cpp
        Source/Components/DrawComponents/DrawPolygonComponent.h
        Source/Actors/Player.cpp
        Source/Actors/Player.h
        Source/Components/ColliderComponents/AABBColliderComponent.cpp
        Source/Components/ColliderComponents/AABBColliderComponent.h
        Source/Components/DrawComponents/DrawAnimatedComponent.cpp
        Source/Components/DrawComponents/DrawAnimatedComponent.h
        Source/Actors/Block.cpp
        Source/Actors/Block.h
        Source/AudioSystem.cpp
        Source/AudioSystem.h
        Source/UIElements/UIScreen.h
        Source/UIElements/UIScreen.cpp
        Source/UIElements/UIFont.h
        Source/UIElements/UIFont.cpp
        Source/UIElements/UIButton.cpp
        Source/UIElements/UIButton.h
        Source/UIElements/UIText.cpp
        Source/UIElements/UIText.h
        Source/UIElements/UIElement.cpp
        Source/UIElements/UIElement.h
        Source/HUD.cpp
        Source/UIElements/UIImage.cpp
        Source/UIElements/UIImage.h
        Source/SpatialHashing.cpp
        Source/SpatialHashing.h
        Source/Components/PowerupComponents/TimePowerupComponent.cpp
        Source/Components/PowerupComponents/TimePowerupComponent.h
        Source/Actors/EnemyBlock.cpp
        Source/Actors/EnemyBlock.h
        Source/Actors/PressMachine.cpp
        Source/Actors/RobotPlane.cpp
        Source/Actors/RobotPlane.h
        Source/Actors/Missile.cpp
        Source/Actors/Missile.h
        Source/Actors/MissileSpawner.cpp
        Source/Actors/MissileSpawner.h
        Source/Actors/PauseMenu.cpp
        Source/Actors/PauseMenu.h
        Source/Actors/StoryScreen.cpp
        Source/Actors/StoryScreen.h
        Source/Actors/Collectible.cpp
        Source/Actors/Collectible.h
        Source/Components/PowerupComponents/PowerupComponent.cpp
        Source/Components/PowerupComponents/PowerupComponent.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2_mixer::SDL2_mixer)
