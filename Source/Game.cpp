// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "CSV.h"
#include "Random.h"
#include "Game.h"
#include "HUD.h"
#include "SpatialHashing.h"
#include "Actors/Actor.h"
#include "Actors/Mario.h"
#include "Actors/Block.h"
#include "Actors/EnemyBlock.h"
#include "Actors/Collectible.h"
#include "Actors/Spawner.h"
#include "UIElements/UIScreen.h"
#include "Components/DrawComponents/DrawComponent.h"
#include "Components/DrawComponents/DrawSpriteComponent.h"
#include "Components/DrawComponents/DrawPolygonComponent.h"
#include "Components/ColliderComponents/AABBColliderComponent.h"
#include "Actors/PressMachine.h"
#include "Actors/RobotPlane.h"

Game::Game(int windowWidth, int windowHeight)
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mWindowWidth(windowWidth)
        ,mWindowHeight(windowHeight)
        ,mMario(nullptr)
        ,mRobotPlane(nullptr)
        ,mHUD(nullptr)
        ,mBackgroundColor(0, 0, 0)
        ,mModColor(255, 255, 255)
        ,mCameraPos(Vector2::Zero)
        ,mAudio(nullptr)
        ,mGameTimer(0.0f)
        ,mGameTimeLimit(0)
        ,mSceneManagerTimer(0.0f)
        ,mSceneManagerState(SceneManagerState::None)
        ,mGameScene(GameScene::MainMenu)
        ,mNextScene(GameScene::MainMenu)
        ,mBackgroundTexture(nullptr)
        ,mBackgroundSize(Vector2::Zero)
        ,mBackgroundPosition(Vector2::Zero)
{

}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("ByteGuard", 20, 30, mWindowWidth, mWindowHeight, 0);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!mRenderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        SDL_Log("Failed to initialize SDL_mixer");
        return false;
    }

    // Start random number generator
    Random::Init();

    mAudio = new AudioSystem();

    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f,
                                         LEVEL_WIDTH * TILE_SIZE,
                                         LEVEL_HEIGHT * TILE_SIZE);
    mTicksCount = SDL_GetTicks();

    // Init all game actors
    //SetGameScene(GameScene::MainMenu, 0);
    //SetGameScene(GameScene::Level1);
    SetGameScene(GameScene::Level2);
    mParallaxCache[GameScene::Level1] = {
        LoadTexture("../Assets/Sprites/Background/1.png"),
        LoadTexture("../Assets/Sprites/Background/2.png"),
        LoadTexture("../Assets/Sprites/Background/3.png"),
        LoadTexture("../Assets/Sprites/Background/4.png"),
        LoadTexture("../Assets/Sprites/Background/5.png")
    };

    mParallaxCache[GameScene::Level2] = {
        LoadTexture("../Assets/Sprites/Background/1_Level_2.png"),
        LoadTexture("../Assets/Sprites/Background/2_Level_2.png"),
        LoadTexture("../Assets/Sprites/Background/3_Level_2.png"),
        LoadTexture("../Assets/Sprites/Background/4_Level_2.png"),
        LoadTexture("../Assets/Sprites/Background/5_Level_2.png")
    };

    /*mParallax3 = LoadTexture("../Assets/Sprites/Background/3.png");
    mParallax4 = LoadTexture("../Assets/Sprites/Background/4.png");
    mParallax5 = LoadTexture("../Assets/Sprites/Background/5.png");
    mBg1 = LoadTexture("../Assets/Sprites/Background/1.png");
    mBg2 = LoadTexture("../Assets/Sprites/Background/2.png");*/

    return true;
}

void Game::SetGameScene(Game::GameScene scene, float transitionTime)
{
    if (mSceneManagerState == SceneManagerState::None)
    {
        if (scene == GameScene::MainMenu || scene == GameScene::Level1 || scene == GameScene::Level2)
        {
            mNextScene = scene;
            mSceneManagerState = SceneManagerState::Entering;
            mSceneManagerTimer = transitionTime;
            if (scene == GameScene::Level1 || scene == GameScene::Level2) {
                mCurrentParallax = &mParallaxCache[scene];
            }
        }
        else
        {
            SDL_Log("Invalid scene passed to SetGameScene().");
            return;
        }
    }
    else
    {
        SDL_Log("Scene transition already underway. Ignoring new order.");
        return;
    }
}

void Game::ResetGameScene(float transitionTime)
{
    mCoinCount = 0;

    if (mSceneAttempts.find(mGameScene) != mSceneAttempts.end()) {
        mSceneAttempts[mGameScene]++;
    } else {
        mSceneAttempts[mGameScene] = 1;
    }

    SetGameScene(mGameScene, transitionTime);
}

void Game::ChangeScene()
{
    // Unload current Scene
    UnloadScene();

    // Reset camera position
    mCameraPos.Set(0.0f, 0.0f);

    // Reset game timer
    mGameTimer = 0.0f;

    // Reset gameplau state
    mGamePlayState = GamePlayState::Playing;

    // Reset scene manager state
    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f, LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE);

    if (mSceneAttempts.find(mNextScene) == mSceneAttempts.end()) {
        mSceneAttempts[mNextScene] = 1;
    }

    // Scene Manager FSM: using if/else instead of switch
    if (mNextScene == GameScene::MainMenu)
    {
        // Set background color
        mBackgroundColor.Set(5.0f, 12.0f, 22.0f);

        // Initialize main menu actors
        LoadMainMenu();
    }
    else if (mNextScene == GameScene::Level1)
    {
        mHUD = new HUD(mRenderer, this, "../Assets/Fonts/Rajdhani-Bold.ttf");

        mGameTimeLimit = 400;
        int tentativa = mSceneAttempts[mNextScene];

        mHUD->SetAttemptCount(tentativa);

        mMusicHandle = mAudio->PlaySound("Main_Level1.mp3", true);

        // Set background color
        mBackgroundColor.Set(55.0f, 68.0f, 110.0f);

        // Set background color
        SetBackgroundImage("../Assets/Sprites/Background_Level1.png", Vector2(0,0), Vector2(1600,600));

        LoadLevel("../Assets/Levels/level1.csv", LEVEL_WIDTH, LEVEL_HEIGHT, 1);
    }
    else if (mNextScene == GameScene::Level2)
    {
        mHUD = new HUD(mRenderer, this, "../Assets/Fonts/Rajdhani-Bold.ttf");

        int tentativa = mSceneAttempts[mNextScene];

        mHUD->SetAttemptCount(tentativa);

        mMusicHandle = mAudio->PlaySound("MusicUnderground.ogg", true);

        // Set background color
        mBackgroundColor.Set(128,24,27);

        // Initialize actors
        LoadLevel("../Assets/Levels/level2.csv", LEVEL_WIDTH, LEVEL_HEIGHT, 2);
    }

    // Set new scene
    mGameScene = mNextScene;
}


void Game::LoadMainMenu()
{
    auto mainMenu = new UIScreen(this, "../Assets/Fonts/Rajdhani-Bold.ttf");

    const Vector2 menuImagePos = Vector2(0,0);
    const Vector2 menuImageDims= Vector2(mWindowWidth,450);

    mainMenu->AddImage(mRenderer, "../Assets/Sprites/Logo_ByteGuard.png", menuImagePos, menuImageDims);

    mMusicHandle = mAudio->PlaySound("Menu_Music.mp3", true);

    mainMenu->AddButton(
        "Jogar",
        Vector2(mWindowWidth/2.0f - 150.0f, mWindowHeight/4.0f * 3.0f),
        Vector2(300.0f, 60.0f),
        [this]() {
            SetGameScene(GameScene::Level1);
            this->GetAudio()->StopAllSounds();
            mMusicHandle = mAudio->PlaySound("Click_Jogar.ogg", false);
    });
}

void Game::LoadLevel(const std::string& levelName, const int levelWidth, const int levelHeight, int level)
{
    // Load level data
    int **mLevelData = ReadLevelData(levelName, levelWidth, levelHeight);

    if (!mLevelData) {
        SDL_Log("Failed to load level data");
        return;
    }

    // Instantiate level actors
    if (level == 1){
        BuildFirstLevel(mLevelData, levelWidth, levelHeight);
    }
    else if (level == 2) {
        BuildSecondLevel(mLevelData, levelWidth, levelHeight);
    }
}

void Game::BuildFirstLevel(int** levelData, int width, int height)
{

    // Const map to convert tile ID to block type
    const std::map<int, const std::string> tileMap = {
        {0, "../Assets/Sprites/ScifiBlocks/Acid_1.png"},
        {1, "../Assets/Sprites/ScifiBlocks/BGTile_1.png"},
        {2, "../Assets/Sprites/ScifiBlocks/BGTile_4.png"},
        {3, "../Assets/Sprites/ScifiBlocks/Fence_1.png"},
        {4, "../Assets/Sprites/ScifiBlocks/Spike2.png"},
        {5, "../Assets/Sprites/ScifiBlocks/Spike8.png"},
        {6, "../Assets/Sprites/ScifiBlocks/Tile_15.png"},
        {7, "../Assets/Sprites/ScifiBlocks/Tile_9.png"},
        {8, "../Assets/Sprites/ScifiBlocks/Acid_2.png"},
        {9, "../Assets/Sprites/ScifiBlocks/BGTile_6.png"},
        {10, "../Assets/Sprites/ScifiBlocks/BGTile_7.png"},
        {11, "../Assets/Sprites/ScifiBlocks/Spike1.png"},
        {12, "../Assets/Sprites/ScifiBlocks/Spike7.png"},
        {13, "../Assets/Sprites/ScifiBlocks/Tile_14.png"},
        {14, "../Assets/Sprites/ScifiBlocks/Tile_8.png"},
        {16, "../Assets/Sprites/ScifiBlocks/BGTile_2.png"},
        {17, "../Assets/Sprites/ScifiBlocks/BGTile_6.png"},
        {18, "../Assets/Sprites/ScifiBlocks/Spike0.png"},
        {19, "../Assets/Sprites/ScifiBlocks/Spike6.png"},
        {20, "../Assets/Sprites/ScifiBlocks/Tile_14.png"},
        {21, "../Assets/Sprites/ScifiBlocks/Tile_7.png"},
        {24, "../Assets/Sprites/ScifiBlocks/BGTile_5.png"},
        {25, "../Assets/Sprites/ScifiBlocks/Fence_3.png"},
        {26, "../Assets/Sprites/ScifiBlocks/Spike5.png"},
        {27, "../Assets/Sprites/ScifiBlocks/Tile_15.png"},
        {28, "../Assets/Sprites/ScifiBlocks/Tile_6.png"},
        {32, "../Assets/Sprites/ScifiBlocks/Fence_2.png"},
        {33, "../Assets/Sprites/ScifiBlocks/Spike4.png"},
        {34, "../Assets/Sprites/ScifiBlocks/Tile_11.png"},
        {35, "../Assets/Sprites/ScifiBlocks/Tile_5.png"},
        {40, "../Assets/Sprites/ScifiBlocks/Spike3.png"},
        {41, "../Assets/Sprites/ScifiBlocks/Tile_10.png"},
        {42, "../Assets/Sprites/ScifiBlocks/Tile_4.png"},
        {48, "../Assets/Sprites/ScifiBlocks/Tile_1.png"},
        {49, "../Assets/Sprites/ScifiBlocks/Tile_3.png"},
        {56, "../Assets/Sprites/ScifiBlocks/Tile_2.png"},
    };

    for (int y = 0; y < LEVEL_HEIGHT; ++y)
    {
        for (int x = 0; x < LEVEL_WIDTH; ++x)
        {
            int tile = levelData[y][x];

            if(tile == 15) // Mario
            {
                mMario = new Mario(this);
                mMario->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            /*else if(tile == 3)
            {
                Collectible* coin = new Collectible(this);
                coin->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }*/
            else if(tile == 36)
            {
                auto* press = new PressMachine(this, mRenderer);
                press->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if (
                    tile == 0  || tile == 8  ||
                    tile == 4  || tile == 5  || tile == 11 || tile == 12 ||
                    tile == 18 || tile == 19 || tile == 26 || tile == 33 || tile == 40
                ) {
                auto it = tileMap.find(tile);
                if (it != tileMap.end())
                {
                    EnemyBlock* enemyBlock= new EnemyBlock(this, it->second);
                    enemyBlock->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                }
            }
            else
            {
                auto it = tileMap.find(tile);
                if (it != tileMap.end())
                {
                    Block* block = new Block(this, it->second);
                    block->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                }
            }
        }
    }
}

void Game::BuildSecondLevel(int** levelData, int width, int height)
{

    // Const map to convert tile ID to block type
     const std::map<int, const std::string> tileMap = {
        {0, "../Assets/Sprites/Level_2_Tileset/Acid_1.png"},
        {1, "../Assets/Sprites/Level_2_Tileset/Barrel1.png"},
        {2, "../Assets/Sprites/Level_2_Tileset/Entry.png"},
        {3, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_23.png"},
        {4, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_54.png"},
        {5, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_63.png"},
        {6, "../Assets/Sprites/Level_2_Tileset/Spike_12.png"},
        {7, "../Assets/Sprites/Level_2_Tileset/Spike_2.png"},
        {8, "../Assets/Sprites/Level_2_Tileset/Acid_2.png"},
        {9, "../Assets/Sprites/Level_2_Tileset/Box2.png"},
        {10, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_11.png"},
        {11, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_52.png"},
        {12, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_62.png"},
        {13, "../Assets/Sprites/Level_2_Tileset/Spike_11.png"},
        {14, "../Assets/Sprites/Level_2_Tileset/Spike_19.png"},
        {15, "../Assets/Sprites/Level_2_Tileset/Spike_9.png"},
        {16, "../Assets/Sprites/Level_2_Tileset/Box1.png"},
        {18, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_45.png"},
        {19, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_61.png"},
        {20, "../Assets/Sprites/Level_2_Tileset/Spike_10.png"},
        {21, "../Assets/Sprites/Level_2_Tileset/Spike_18.png"},
        {22, "../Assets/Sprites/Level_2_Tileset/Spike_8.png"},
        {25, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_36.png"},
        {26, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_57.png"},
        {27, "../Assets/Sprites/Level_2_Tileset/Spike_1.png"},
        {28, "../Assets/Sprites/Level_2_Tileset/Spike_17.png"},
        {29, "../Assets/Sprites/Level_2_Tileset/Spike_7.png"},
        {32, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_32.png"},
        {33, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_56.png"},
        {34, "../Assets/Sprites/Level_2_Tileset/Locker4.png"},
        {35, "../Assets/Sprites/Level_2_Tileset/Spike_16.png"},
        {36, "../Assets/Sprites/Level_2_Tileset/Spike_6.png"},
        {40, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_55.png"},
        {41, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_71.png"},
        {42, "../Assets/Sprites/Level_2_Tileset/Spike_15.png"},
        {43, "../Assets/Sprites/Level_2_Tileset/Spike_5.png"},
        {48, "../Assets/Sprites/Level_2_Tileset/IndustrialTile_70.png"},
        {49, "../Assets/Sprites/Level_2_Tileset/Spike_14.png"},
        {50, "../Assets/Sprites/Level_2_Tileset/Spike_4.png"},
        {56, "../Assets/Sprites/Level_2_Tileset/Spike_13.png"},
        {57, "../Assets/Sprites/Level_2_Tileset/Spike_3.png"},
        {64, "../Assets/Sprites/Level_2_Tileset/Spike_20.png"},
    };

    for (int y = 0; y < LEVEL_HEIGHT; ++y)
    {
        for (int x = 0; x < LEVEL_WIDTH; ++x)
        {
            int tile = levelData[y][x];

            if(tile == 17) // Mario
            {
                mRobotPlane = new RobotPlane(this);
                mRobotPlane->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            /*else if(tile == 3)
            {
                Collectible* coin = new Collectible(this);
                coin->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }*/
            else if(tile == 24)
            {
                auto* press = new PressMachine(this, mRenderer);
                press->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if (
                tile == 0 || tile == 8 ||
                tile == 6 || tile == 7 || tile == 13 || tile == 14 || tile == 15 ||
                tile == 20 || tile == 21 || tile == 22 || tile == 27 || tile == 28 ||
                tile == 29 || tile == 35 || tile == 36 || tile == 42 || tile == 43 ||
                tile == 49 || tile == 50 || tile == 56 || tile == 57 || tile == 64 )
            {
                auto it = tileMap.find(tile);
                if (it != tileMap.end())
                {
                    EnemyBlock* enemyBlock= new EnemyBlock(this, it->second);
                    enemyBlock->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                }
            }
            else // Blocks
            {
                auto it = tileMap.find(tile);
                if (it != tileMap.end())
                {
                    // Create a block actor
                    Block* block = new Block(this, it->second);
                    block->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                }
            }
        }
    }
}

int **Game::ReadLevelData(const std::string& fileName, int width, int height)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        SDL_Log("Failed to load paths: %s", fileName.c_str());
        return nullptr;
    }

    // Create a 2D array of size width and height to store the level data
    int** levelData = new int*[height];
    for (int i = 0; i < height; ++i)
    {
        levelData[i] = new int[width];
    }

    // Read the file line by line
    int row = 0;

    std::string line;
    while (!file.eof())
    {
        std::getline(file, line);
        if(!line.empty())
        {
            auto tiles = CSVHelper::Split(line);

            if (tiles.size() != width) {
                SDL_Log("Invalid level data");
                return nullptr;
            }

            for (int i = 0; i < width; ++i) {
                levelData[row][i] = tiles[i];
            }
        }

        ++row;
    }

    // Close the file
    file.close();

    return levelData;
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;
            case SDL_KEYDOWN:
                // Handle key press for UI screens
                if (!mUIStack.empty()) {
                    mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
                }

                HandleKeyPressActors(event.key.keysym.sym, event.key.repeat == 0);

                // Check if the Return key has been pressed to pause/unpause the game
                if (event.key.keysym.sym == SDLK_RETURN)
                {
                    TogglePause();
                }
                break;
        }
    }

    ProcessInputActors();
}

void Game::ProcessInputActors()
{
    if(mGamePlayState == GamePlayState::Playing)
    {
        // Get actors on camera
        std::vector<Actor*> actorsOnCamera =
                mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

        const Uint8* state = SDL_GetKeyboardState(nullptr);

        bool isMarioOnCamera = false;
        for (auto actor: actorsOnCamera)
        {
            actor->ProcessInput(state);

            if (actor == mMario || actor == mRobotPlane) {
                isMarioOnCamera = true;
            }
        }

        // If Mario is not on camera, process input for him
        if (!isMarioOnCamera && mMario) {
            mMario->ProcessInput(state);
        } else if (!isMarioOnCamera && mRobotPlane) {
            mRobotPlane->ProcessInput(state);
        }
    }
}

void Game::HandleKeyPressActors(const int key, const bool isPressed)
{
    if(mGamePlayState == GamePlayState::Playing)
    {
        // Get actors on camera
        std::vector<Actor*> actorsOnCamera =
                mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

        // Handle key press for actors
        bool isMarioOnCamera = false;
        for (auto actor: actorsOnCamera) {
            actor->HandleKeyPress(key, isPressed);

            if (actor == mMario || actor == mRobotPlane) {
                isMarioOnCamera = true;
            }
        }

        // If Mario is not on camera, handle key press for him
        if (!isMarioOnCamera && mMario)
        {
            mMario->HandleKeyPress(key, isPressed);
        } else if (!isMarioOnCamera && mRobotPlane)
        {
            mRobotPlane->HandleKeyPress(key, isPressed);
        }
    }

}

void Game::TogglePause()
{
    if (mGameScene != GameScene::MainMenu)
    {
        if (mGamePlayState == GamePlayState::Playing)
        {
            mGamePlayState = GamePlayState::Paused;

            // --------------
            // TODO - PARTE 4
            // --------------

            // TODO 1.: Pare a música de fundo atual usando PauseSound() e toque o som "Coin.wav" para indicar a pausa.
            mAudio->PauseSound(mMusicHandle);

        }
        else if (mGamePlayState == GamePlayState::Paused)
        {
            mGamePlayState = GamePlayState::Playing;

            // --------------
            // TODO - PARTE 4
            // --------------

            // TODO 1.: Retome a música de fundo atual usando ResumeSound() e toque o som "Coin.wav" para
            //  indicar a retomada do jogo.
            mAudio->ResumeSound(mMusicHandle);
        }
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    mTicksCount = SDL_GetTicks();

    if(mGamePlayState != GamePlayState::Paused && mGamePlayState != GamePlayState::GameOver)
    {
        // Reinsert all actors and pending actors
        UpdateActors(deltaTime);
    }

    // Reinsert audio system
    mAudio->Update(deltaTime);

    // Reinsert UI screens
    for (auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    // Delete any UIElements that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }

    // ---------------------
    // Game Specific Updates
    // ---------------------
    UpdateCamera();

    // --------------
    // TODO - PARTE 2
    // --------------

    // TODO 1.: Chame UpdateSceneManager passando o deltaTime.
    UpdateSceneManager(deltaTime);

    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 1.: Verifique se a cena atual é diferente de GameScene::MainMenu e se o estado do jogo é
    //  GamePlayState::Playing. Se sim, chame UpdateLevelTime passando o deltaTime.
    if (mGameScene != GameScene::MainMenu && mGamePlayState == GamePlayState::Playing)
    {
        UpdateLevelTime(deltaTime);
    }
}

void Game::UpdateSceneManager(float deltaTime)
{
    // --------------
    // TODO - PARTE 2
    // --------------

    // TODO 1.: Verifique se o estado do SceneManager é SceneManagerState::Entering. Se sim, decremente o mSceneManagerTimer
    //  usando o deltaTime. Em seguida, verifique se o mSceneManagerTimer é menor ou igual a 0. Se for, reinicie o
    //  mSceneManagerTimer para TRANSITION_TIME e mude o estado do SceneManager para SceneManagerState::Active.
    if (mSceneManagerState == SceneManagerState::Entering)
    {
        mSceneManagerTimer -= deltaTime;

        if (mSceneManagerTimer <= 0.0f)
        {
            ChangeScene();
            mSceneManagerTimer = TRANSITION_TIME;
            mSceneManagerState = SceneManagerState::Active;
        }
    }

    // TODO 2.: Verifique se o estado do SceneManager é SceneManagerState::Active. Se sim, decremente o mSceneManagerTimer
    //  usando o deltaTime. Em seguida, verifique se o mSceneManagerTimer é menor ou igual a 0. Se for, chame ChangeScene()
    //  e mude o estado do SceneManager para SceneManagerState::None.
    else if (mSceneManagerState == SceneManagerState::Active)
    {
        mSceneManagerTimer -= deltaTime;

        if (mSceneManagerTimer <= 0.0f)
        {
            mSceneManagerState = SceneManagerState::None;
        }
    }

    // TODO 3.: Remova a chamada da função ChangeScene() do método Initialize(), pois ela será chamada automaticamente
    //  durante o UpdateSceneManager() quando o estado do SceneManager for SceneManagerState::Active.
}

void Game::UpdateLevelTime(float deltaTime)
{
    mGameTimer += deltaTime;

    if (mGameTimer >= 1.0f)
    {
        mGameTimer = 0.0f;
        mGameTimeLimit--;

        if (mHUD)
        {
            //mHUD->SetTime(mGameTimeLimit);
        }
        /*if (mGameTimeLimit <= 0 && mMario)
        {
            mMario->Kill();
        }*/
    }
}

void Game::UpdateCamera()
{
    if (!mMario && !mRobotPlane ) return;

    float horizontalCameraPos;

    if (mGameScene == GameScene::Level1) {
        horizontalCameraPos = mMario->GetPosition().x - (mWindowWidth / 2.0f);
    }
    else if (mGameScene == GameScene::Level2) {
        horizontalCameraPos = mRobotPlane->GetPosition().x - (mWindowWidth / 2.0f);
    }
    else {
        return;
    }

    if (horizontalCameraPos > mCameraPos.x)
    {
        float maxCameraPos = (LEVEL_WIDTH * TILE_SIZE) - mWindowWidth;
        horizontalCameraPos = Math::Clamp(horizontalCameraPos, 0.0f, maxCameraPos);

        mCameraPos.x = horizontalCameraPos;
    }
}

void Game::UpdateActors(float deltaTime)
{
    // Get actors on camera
    std::vector<Actor*> actorsOnCamera =
            mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

    bool isMarioOnCamera = false;
    for (auto actor : actorsOnCamera)
    {
        actor->Update(deltaTime);
        if ((mMario && actor == mMario) || (mRobotPlane && actor == mRobotPlane))
        {
            isMarioOnCamera = true;
        }
    }

    // If Mario is not on camera, reset camera position
    if (!isMarioOnCamera && mMario)
    {
        mMario->Update(deltaTime);
    }
    else if (!isMarioOnCamera && mRobotPlane)
    {
        mRobotPlane->Update(deltaTime);
    }

    for (auto actor : actorsOnCamera)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            delete actor;
            if (actor == mMario) {
                mMario = nullptr;
            }
            else if (actor == mRobotPlane) {
                mRobotPlane = nullptr;
            }
        }
    }
}

void Game::AddActor(Actor* actor)
{
    mSpatialHashing->Insert(actor);
}

void Game::RemoveActor(Actor* actor)
{
    mSpatialHashing->Remove(actor);
}
void Game::Reinsert(Actor* actor)
{
    mSpatialHashing->Reinsert(actor);
}

std::vector<Actor *> Game::GetNearbyActors(const Vector2& position, const int range)
{
    return mSpatialHashing->Query(position, range);
}

std::vector<AABBColliderComponent *> Game::GetNearbyColliders(const Vector2& position, const int range)
{
    return mSpatialHashing->QueryColliders(position, range);
}

void Game::GenerateOutput()
{
    // Clear frame with background color
    SDL_SetRenderDrawColor(mRenderer, mBackgroundColor.x, mBackgroundColor.y, mBackgroundColor.z, 255);

    // Clear back buffer
    SDL_RenderClear(mRenderer);

    if (mGameScene == GameScene::Level1 || mGameScene == GameScene::Level2) {
        DrawParallaxBackground(mRenderer, mCameraPos);
    }


    // Get actors on camera
    std::vector<Actor*> actorsOnCamera =
            mSpatialHashing->QueryOnCamera(mCameraPos,mWindowWidth,mWindowHeight);

    // Get list of drawables in draw order
    std::vector<DrawComponent*> drawables;

    for (auto actor : actorsOnCamera)
    {
        auto drawable = actor->GetComponent<DrawComponent>();
        if (drawable && drawable->IsVisible())
        {
            drawables.emplace_back(drawable);
        }
    }

    // Sort drawables by draw order
    std::sort(drawables.begin(), drawables.end(),
              [](const DrawComponent* a, const DrawComponent* b) {
                  return a->GetDrawOrder() < b->GetDrawOrder();
              });

    // Draw all drawables
    for (auto drawable : drawables)
    {
        drawable->Draw(mRenderer, mModColor);
    }

    // Draw all UI screens
    for (auto ui :mUIStack)
    {
        ui->Draw(mRenderer);
    }

    // --------------
    // TODO - PARTE 2
    // --------------

    // TODO 1.: Verifique se o SceneManager está no estado ativo. Se estiver, desenhe um retângulo preto cobrindo
    //  toda a tela.

    // Customização 2: alterações para fazer o crosfade
    if (mSceneManagerState == SceneManagerState::Entering || mSceneManagerState == SceneManagerState::Active)
    {
        float alphaRatio = mSceneManagerTimer / TRANSITION_TIME;
        if (alphaRatio < 0.0f) alphaRatio = 0.0f;
        if (alphaRatio > 1.0f) alphaRatio = 1.0f;

        Uint8 alpha = 255;

        if (mSceneManagerState == SceneManagerState::Entering)
        {
            alpha = static_cast<Uint8>((1.0f - alphaRatio) * 255.0f);
        }
        else if (mSceneManagerState == SceneManagerState::Active)
        {
            alpha = static_cast<Uint8>(alphaRatio * 255.0f);
        }

        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, alpha);
        SDL_Rect blackRect = { 0, 0, mWindowWidth, mWindowHeight };
        SDL_RenderFillRect(mRenderer, &blackRect);

        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_NONE);
    }

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

void Game::SetBackgroundImage(const std::string& texturePath, const Vector2 &position, const Vector2 &size)
{
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }

    // Load background texture
    mBackgroundTexture = LoadTexture(texturePath);
    if (!mBackgroundTexture) {
        SDL_Log("Failed to load background texture: %s", texturePath.c_str());
    }

    // Set background position
    mBackgroundPosition.Set(position.x, position.y);

    // Set background size
    mBackgroundSize.Set(size.x, size.y);
}

SDL_Texture* Game::LoadTexture(const std::string& texturePath)
{
    SDL_Surface* surface = IMG_Load(texturePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }

    return texture;
}

void Game::AddCoin() {
    mCoinCount++;
    if (mHUD) {
        mHUD->SetCoinsCount(mCoinCount);
    }
}


UIFont* Game::LoadFont(const std::string& fileName)
{
    // --------------
    // TODO - PARTE 1-1
    // --------------

    // TODO 1.: Verifique se o arquivo de fonte já está carregado no mapa mFonts.
    //  Se sim, retorne o ponteiro para a fonte carregada.
    //  Se não, crie um novo objeto UIFont, carregue a fonte do arquivo usando o método Load,
    //  e se o carregamento for bem-sucedido, adicione a fonte ao mapa mFonts.
    //  Se o carregamento falhar, descarregue a fonte com Unload e delete o objeto UIFont, retornando nullptr.
    auto iter = mFonts.find(fileName);
    if (iter != mFonts.end())
    {
        return iter->second;
    }

    UIFont* newFont = new UIFont(mRenderer);
    if (newFont->Load(fileName))
    {
        mFonts[fileName] = newFont;
        return newFont;
    }
    else
    {
        newFont->Unload();
        delete newFont;
        SDL_Log("Failed to load font: %s", fileName.c_str());
        return nullptr;
    }
}

void Game::DrawParallaxBackground(SDL_Renderer* renderer, const Vector2& cameraPos)
{

    SDL_Texture* fixedBg1 = mCurrentParallax->bg1;
    SDL_Texture* fixedBg2 = mCurrentParallax->bg2;

    int texW1, texH1;
    SDL_QueryTexture(fixedBg1, nullptr, nullptr, &texW1, &texH1);
    SDL_Rect dst1 = { 0, 0, texW1, texH1 };
    SDL_RenderCopy(renderer, fixedBg1, nullptr, &dst1);

    int texW2, texH2;
    SDL_QueryTexture(fixedBg2, nullptr, nullptr, &texW2, &texH2);
    SDL_Rect dst2 = { 0, 0, texW2, texH2 };
    SDL_RenderCopy(renderer, fixedBg2, nullptr, &dst2);

    struct Layer {
        SDL_Texture* texture;
        float parallaxFactor;
    };

    std::vector<Layer> layers = {
        {mCurrentParallax->layer3, 0.1f},
        {mCurrentParallax->layer4, 0.3f},
        {mCurrentParallax->layer5, 0.5f}
    };

    int screenWidth = GetWindowWidth();

    for (auto& layer : layers)
    {
        int texW, texH;
        SDL_QueryTexture(layer.texture, nullptr, nullptr, &texW, &texH);

        float scrollX = fmod(cameraPos.x * layer.parallaxFactor, static_cast<float>(texW));
        if (scrollX < 0) scrollX += texW;

        int numTiles = screenWidth / texW + 2;

        for (int i = 0; i < numTiles; ++i)
        {
            SDL_Rect dst = {
                static_cast<int>(-scrollX + i * texW),
                0,
                texW,
                texH
            };

            SDL_RenderCopy(renderer, layer.texture, nullptr, &dst);
        }
    }
}

void Game::UnloadScene()
{
    // Delete actors
    delete mSpatialHashing;

    // Delete UI screens
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    // Delete background texture
    if (mBackgroundTexture) {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }
}

void Game::Shutdown()
{
    UnloadScene();

    for (auto font : mFonts) {
        font.second->Unload();
        delete font.second;
    }
    mFonts.clear();

    delete mAudio;
    mAudio = nullptr;

    Mix_CloseAudio();

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();

    for (auto& [scene, parallax] : mParallaxCache)
    {
        SDL_DestroyTexture(parallax.bg1);
        SDL_DestroyTexture(parallax.bg2);
        SDL_DestroyTexture(parallax.layer3);
        SDL_DestroyTexture(parallax.layer4);
        SDL_DestroyTexture(parallax.layer5);
    }
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
