// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include "CSV.h"
#include "HUD.h"
#include "Random.h"
#include "SpatialHashing.h"
#include "Actors/Actor.h"
#include "Actors/Block.h"
#include "Actors/Collectible.h"
#include "Actors/EnemyBlock.h"
#include "Actors/GameWinScreen.h"
#include "Actors/LaserSpikes.h"
#include "Actors/LaserTurret.h"
#include "Actors/PauseMenu.h"
#include "Actors/MissileSpawner.h"
#include "Actors/Player.h"
#include "Actors/PressMachine.h"
#include "Actors/RobotPlane.h"
#include "Actors/Saw.h"
#include "Actors/StoryScreen.h"
#include "Actors/TutorialGuide.h"
#include "Actors/WallBlades.h"
#include "Components/PowerupComponents/TimePowerupComponent.h"
#include "Components/ColliderComponents/AABBColliderComponent.h"
#include "Components/DrawComponents/DrawComponent.h"
#include "UIElements/UIScreen.h"

Game::Game(const int windowWidth, const int windowHeight)
    : mSceneManagerState(SceneManagerState::None),
      mSceneManagerTimer(0.0f),
      mSpatialHashing(nullptr),
      mWindow(nullptr),
      mRenderer(nullptr),
      mAudio(nullptr),
      mWindowWidth(windowWidth),
      mWindowHeight(windowHeight),
      mTicksCount(0),
      mIsRunning(true),
      mGamePlayState(GamePlayState::Playing),
      mGameScene(GameScene::MainMenu),
      mNextScene(GameScene::MainMenu),
      mBackgroundColor(0, 0, 0),
      mModColor(255, 255, 255),
      mCameraPos(Vector2::Zero), mPlayer(nullptr),
      mRobotPlane(nullptr),
      mHUD(nullptr),
      mGameTimer(0.0f),
      mGameTimeLimit(0),
      mBackgroundTexture(nullptr),
      mBackgroundSize(Vector2::Zero),
      mBackgroundPosition(Vector2::Zero),
      mSlowingFactor(1.0f) {}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("ByteGuard",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               mWindowWidth,
                               mWindowHeight, 0);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1,
                                   SDL_RENDERER_ACCELERATED |
                                   SDL_RENDERER_PRESENTVSYNC);
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

    // Start random number generator
    Random::Init();

    mAudio = new AudioSystem();

    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f,
                                         LEVEL_WIDTH * TILE_SIZE,
                                         LEVEL_HEIGHT * TILE_SIZE);
    mTicksCount = SDL_GetTicks();

    SetGameScene(GameScene::MainMenu, 0);

    mParallaxCache[GameScene::TutorialLevel] = {
        LoadTexture("../Assets/Sprites/Background/1_Tutorial.png"),
        LoadTexture("../Assets/Sprites/Background/2_Tutorial.png"),
        LoadTexture("../Assets/Sprites/Background/3_Tutorial.png"),
        LoadTexture("../Assets/Sprites/Background/4_Tutorial.png"),
        LoadTexture("../Assets/Sprites/Background/5_Tutorial.png")
        };

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

    return true;
}

void Game::SetGameScene(const GameScene scene, const float transitionTime)
{
    if (mSceneManagerState == SceneManagerState::None)
    {
        if (scene == GameScene::MainMenu ||
            scene == GameScene::StoryScreen ||
            scene == GameScene::TutorialLevel ||
            scene == GameScene::Level1 ||
            scene == GameScene::Level2 ||
            scene == GameScene::GameWinScreen )
        {
            mNextScene = scene;
            mSceneManagerState = SceneManagerState::Entering;
            mSceneManagerTimer = transitionTime;
        }
        else { SDL_Log("Invalid scene passed to SetGameScene()."); }
    }
    else { SDL_Log("Scene transition already underway. Ignoring new order."); }
}

void Game::ResetGameScene(const float transitionTime)
{
    if (mSceneAttempts.find(mGameScene) != mSceneAttempts.end())
    {
        mSceneAttempts[mGameScene]++;
    }
    else { mSceneAttempts[mGameScene] = 1; }

    SetGameScene(mGameScene, transitionTime);
}

void Game::ChangeScene()
{
    // Unload current Scene
    UnloadScene();

    while (!mUIStack.empty())
    {
        delete mUIStack.back();
        mUIStack.pop_back();
    }

    // Reset camera position
    mCameraPos.Set(0.0f, 0.0f);

    // Reset game timer
    mGameTimer = 0.0f;

    // Reset gameplay state
    mGamePlayState = GamePlayState::Playing;

    // Reset scene manager state
    mSpatialHashing = new SpatialHashing(TILE_SIZE * 4.0f,
                                         LEVEL_WIDTH * TILE_SIZE,
                                         LEVEL_HEIGHT * TILE_SIZE);

    if (mSceneAttempts.find(mNextScene) == mSceneAttempts.end())
    {
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
    else if (mNextScene == GameScene::StoryScreen)
    {
        mBackgroundColor.Set(0.0f, 0.0f, 0.0f);

        new StoryScreen(this, "../Assets/Fonts/Rajdhani-Bold.ttf");
    }
    else if (mNextScene == GameScene::TutorialLevel)
    {
        mHUD = new HUD(mRenderer, this, "../Assets/Fonts/Rajdhani-Bold.ttf");

        mGameTimeLimit = 400;

        const int tryCount = mSceneAttempts[mNextScene];
        mHUD->SetAttemptCount(tryCount);

        mMusicHandle = mAudio->PlaySound("Main_Tutorial.mp3", true);

        // Set background color
        mBackgroundColor.Set(131.0f, 66.0f, 61.0f);
        mCurrentParallax = &mParallaxCache[GameScene::TutorialLevel];

        // Set background image

        LoadLevel("../Assets/Levels/tutorial_level.csv",
                  TUTORIAL_LEVEL_WIDTH,
                  LEVEL_HEIGHT,
                  0);
    }
    else if (mNextScene == GameScene::Level1)
    {
        mHUD = new HUD(mRenderer, this, "../Assets/Fonts/Rajdhani-Bold.ttf");

        mGameTimeLimit = 400;

        const int tryCount = mSceneAttempts[mNextScene];
        mHUD->SetAttemptCount(tryCount);

        mMusicHandle = mAudio->PlaySound("Main_Level1.mp3", true);

        // Set background color
        mBackgroundColor.Set(55.0f, 68.0f, 110.0f);
        mCurrentParallax = &mParallaxCache[GameScene::Level1];

        // Set background image
        SetBackgroundImage("../Assets/Sprites/Background_Level1.png",
                           Vector2(0, 0),
                           Vector2(1600, 600));

        LoadLevel("../Assets/Levels/level1_debug.csv",
                  LEVEL_WIDTH,
                  LEVEL_HEIGHT,
                  1);
    }
    else if (mNextScene == GameScene::Level2)
    {
        mHUD = new HUD(mRenderer, this, "../Assets/Fonts/Rajdhani-Bold.ttf");

        const int tryCount = mSceneAttempts[mNextScene];

        mHUD->SetAttemptCount(tryCount);

        mMusicHandle = mAudio->PlaySound("Musica_Level2.mp3", true);

        // Set background color
        mBackgroundColor.Set(128, 24, 27);
        mCurrentParallax = &mParallaxCache[GameScene::Level2];

        // Initialize actors
        LoadLevel("../Assets/Levels/level2.csv", LEVEL_WIDTH, LEVEL_HEIGHT, 2);
    }
    else if (mNextScene == GameScene::GameWinScreen)
    {
        mBackgroundColor.Set(10.0f, 15.0f, 30.0f);

        //TODO: setar uma música de vitória
        //mAudio->PlaySound("Victory.ogg", false);

        new GameWinScreen(this);
    }

    // Set new scene
    mGameScene = mNextScene;
}


void Game::LoadMainMenu()
{
    const auto mainMenu = new UIScreen(
        this,
        "../Assets/Fonts/Rajdhani-Bold.ttf");

    const auto menuImagePos = Vector2(0, 0);
    const auto menuImageDims = Vector2(mWindowWidth, 450);

    mainMenu->AddImage(mRenderer,
                       "../Assets/Sprites/Logo_ByteGuard.png",
                       menuImagePos,
                       menuImageDims);

    mMusicHandle = mAudio->PlaySound("Menu_Music.mp3", true);

    mainMenu->AddButton(
        "Jogar",
        Vector2(mWindowWidth / 2.0f - 150.0f, mWindowHeight / 4.0f * 3.0f),
        Vector2(300.0f, 60.0f),
        [this]()
        {
            SetGameScene(GameScene::StoryScreen);
            this->GetAudio()->StopAllSounds();
            mMusicHandle = mAudio->PlaySound("Click_Jogar.ogg", false);
        }
    );
}

void Game::LoadLevel(const std::string &levelName,
                     const int levelWidth,
                     const int levelHeight,
                     const int level)
{
    // Load level data
    int **mLevelData = ReadLevelData(levelName, levelWidth, levelHeight);

    if (!mLevelData)
    {
        SDL_Log("Failed to load level data");
        return;
    }

    // Instantiate level actors
    if (level == 0) {
        BuildTutorialLevel(mLevelData, levelWidth, levelHeight);
    }
    else if (level == 1) {
        BuildFirstLevel(mLevelData, levelWidth, levelHeight);
    }
    else if (level == 2)
    {
        BuildSecondLevel(mLevelData, levelWidth, levelHeight);
    }

    for (int i = 0; i < levelHeight; ++i) { delete[] mLevelData[i]; }
    delete[] mLevelData;
}

void Game::BuildTutorialLevel(int **levelData, int width, int height)
{
    // Const map to convert tile ID to block type
    const std::map<int, const std::string> tileMap = {
    {0, "../Assets/Sprites/Tutorial_Tileset/tile000.png"},
    {1, "../Assets/Sprites/Tutorial_Tileset/tile001.png"},
    {2, "../Assets/Sprites/Tutorial_Tileset/tile002.png"},
    {3, "../Assets/Sprites/Tutorial_Tileset/tile003.png"},
    {4, "../Assets/Sprites/Tutorial_Tileset/tile004.png"},
    {5, "../Assets/Sprites/Tutorial_Tileset/tile005.png"},
    {6, "../Assets/Sprites/Tutorial_Tileset/tile006.png"},
    {7, "../Assets/Sprites/Tutorial_Tileset/tile007.png"},
    {8, "../Assets/Sprites/Tutorial_Tileset/tile008.png"},
    {9, "../Assets/Sprites/Tutorial_Tileset/tile009.png"},
    {10, "../Assets/Sprites/Tutorial_Tileset/tile010.png"},
    {11, "../Assets/Sprites/Tutorial_Tileset/tile011.png"},
    {12, "../Assets/Sprites/Tutorial_Tileset/tile012.png"},
    {13, "../Assets/Sprites/Tutorial_Tileset/tile013.png"},
    {14, "../Assets/Sprites/Tutorial_Tileset/tile014.png"},
    {15, "../Assets/Sprites/Tutorial_Tileset/tile015.png"},
    {16, "../Assets/Sprites/Tutorial_Tileset/tile016.png"},
    {17, "../Assets/Sprites/Tutorial_Tileset/tile017.png"},
    {18, "../Assets/Sprites/Tutorial_Tileset/tile018.png"},
    {19, "../Assets/Sprites/Tutorial_Tileset/tile019.png"},
    {20, "../Assets/Sprites/Tutorial_Tileset/tile020.png"},
    {21, "../Assets/Sprites/Tutorial_Tileset/tile021.png"},
    {22, "../Assets/Sprites/Tutorial_Tileset/tile022.png"},
    {23, "../Assets/Sprites/Tutorial_Tileset/tile023.png"},
    {24, "../Assets/Sprites/Tutorial_Tileset/tile024.png"},
    {25, "../Assets/Sprites/Tutorial_Tileset/tile025.png"},
    {26, "../Assets/Sprites/Tutorial_Tileset/tile026.png"},
    {27, "../Assets/Sprites/Tutorial_Tileset/tile027.png"},
    {28, "../Assets/Sprites/Tutorial_Tileset/tile028.png"},
    {29, "../Assets/Sprites/Tutorial_Tileset/tile029.png"},
    {30, "../Assets/Sprites/Tutorial_Tileset/tile030.png"},
    {31, "../Assets/Sprites/Tutorial_Tileset/tile031.png"},
    {32, "../Assets/Sprites/Tutorial_Tileset/tile032.png"},
    {33, "../Assets/Sprites/Tutorial_Tileset/tile033.png"},
    {34, "../Assets/Sprites/Tutorial_Tileset/tile034.png"},
    {35, "../Assets/Sprites/Tutorial_Tileset/Acid_2.png"},
    {36, "../Assets/Sprites/Tutorial_Tileset/tile036.png"},
    {37, "../Assets/Sprites/Tutorial_Tileset/tile037.png"},
    {38, "../Assets/Sprites/Tutorial_Tileset/tile038.png"},
    {39, "../Assets/Sprites/Tutorial_Tileset/tile039.png"},
    {40, "../Assets/Sprites/Tutorial_Tileset/tile040.png"},
    {41, "../Assets/Sprites/Tutorial_Tileset/tile041.png"},
    {42, "../Assets/Sprites/Tutorial_Tileset/Acid_1.png"},
    {43, "../Assets/Sprites/Tutorial_Tileset/tile043.png"},
    {44, "../Assets/Sprites/Tutorial_Tileset/tile044.png"},
    {45, "../Assets/Sprites/Tutorial_Tileset/tile045.png"},
    {46, "../Assets/Sprites/Tutorial_Tileset/tile046.png"},
    {47, "../Assets/Sprites/Tutorial_Tileset/tile047.png"},
    {48, "../Assets/Sprites/Tutorial_Tileset/tile048.png"},
    {49, "../Assets/Sprites/Tutorial_Tileset/tile049.png"},
    {50, "../Assets/Sprites/Tutorial_Tileset/tile050.png"},
    {51, "../Assets/Sprites/Tutorial_Tileset/tile051.png"},
    {52, "../Assets/Sprites/Tutorial_Tileset/tile052.png"},
    {53, "../Assets/Sprites/Tutorial_Tileset/tile053.png"},
    {54, "../Assets/Sprites/Tutorial_Tileset/tile054.png"},
    {55, "../Assets/Sprites/Tutorial_Tileset/tile055.png"},
    {56, "../Assets/Sprites/Tutorial_Tileset/tile056.png"},
    {57, "../Assets/Sprites/Tutorial_Tileset/tile057.png"},
    {58, "../Assets/Sprites/Tutorial_Tileset/tile058.png"},
    {59, "../Assets/Sprites/Tutorial_Tileset/tile059.png"},
    {60, "../Assets/Sprites/Tutorial_Tileset/tile060.png"},
    {61, "../Assets/Sprites/Tutorial_Tileset/tile061.png"},
    {62, "../Assets/Sprites/Tutorial_Tileset/tile062.png"},
    {63, "../Assets/Sprites/Tutorial_Tileset/tile063.png"},
    {64, "../Assets/Sprites/Tutorial_Tileset/tile064.png"},
    {65, "../Assets/Sprites/Tutorial_Tileset/tile065.png"},
    {66, "../Assets/Sprites/Tutorial_Tileset/tile066.png"},
    {67, "../Assets/Sprites/Tutorial_Tileset/tile067.png"},
    {68, "../Assets/Sprites/Tutorial_Tileset/tile068.png"},
    {69, "../Assets/Sprites/Tutorial_Tileset/tile069.png"},
    {70, "../Assets/Sprites/Tutorial_Tileset/tile070.png"},
    {71, "../Assets/Sprites/Tutorial_Tileset/tile071.png"},
    {72, "../Assets/Sprites/Tutorial_Tileset/tile072.png"},
    {74, "../Assets/Sprites/Tutorial_Tileset/tile074.png"},
    {75, "../Assets/Sprites/Tutorial_Tileset/tile075.png"},
    {76, "../Assets/Sprites/Tutorial_Tileset/tile076.png"},
    {77, "../Assets/Sprites/Tutorial_Tileset/tile077.png"},
    {78, "../Assets/Sprites/Tutorial_Tileset/tile078.png"},
    {79, "../Assets/Sprites/Tutorial_Tileset/tile079.png"},
    {80, "../Assets/Sprites/Tutorial_Tileset/tile080.png"}
    };

    for (int y = 0; y < LEVEL_HEIGHT; ++y)
    {
        for (int x = 0; x < TUTORIAL_LEVEL_WIDTH; ++x)
        {
            int tile = levelData[y][x];

            if (tile == 72) // Player
            {
                mPlayer = new Player(this);
                mPlayer->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if (tile == 73)
            {
                const auto spawner = new MissileSpawner(this, 50);
                spawner->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }

            else if (tile == 26)
            {
                const auto timePowerup =
                        new Collectible(this,
                                        Powerups::TimePowerup,
                                        "../Assets/Sprites/Collectibles/Time_Powerup.png");

                timePowerup->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if (tile == 67)
            {
                const auto laserTurret = new LaserTurret(this, mRenderer);
                laserTurret->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }

            else if (tile == 74)
            {
                const auto saw = new Saw(this, mRenderer);
                saw->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if (tile == 75)
            {
                const auto wallBlades = new WallBlades(this, mRenderer);
                wallBlades->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if (
                tile == 78
            )
            {
                const auto laserSpikes = new LaserSpikes(this, mRenderer);
                laserSpikes->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if (
                tile == 42 || tile == 35
            )
            {
                if (const auto it = tileMap.find(tile);
                    it != tileMap.end())
                {
                    // Create an enemy block actor
                    const auto enemyBlock = new EnemyBlock(this, it->second);
                    enemyBlock->SetPosition(
                        Vector2(x * TILE_SIZE, y * TILE_SIZE));
                }
            }
            else
            {
                if (const auto it = tileMap.find(tile);
                    it != tileMap.end())
                {
                    // Create a block actor
                    const auto block = new Block(this, it->second);
                    block->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                }
            }
        }
    }
}

void Game::BuildFirstLevel(int **levelData, int width, int height)
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
            {30, "../Assets/Sprites/Missile/missile.png"},
            {31, "../Assets/Sprites/Missile/missile.png"},
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

            if (tile == 15) // Player
            {
                mPlayer = new Player(this);
                mPlayer->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if (tile == 30)
            {
                const auto block = new Block(this,
                                             "../Assets/Sprites/ScifiBlocks/Tile_6.png");
                block->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                block->GetComponent<AABBColliderComponent>()->SetEnabled(false);

                const auto spawner = new MissileSpawner(this, 50);
                spawner->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if (tile == 31)
            {
                const auto timePowerup =
                        new Collectible(this,
                                        Powerups::TimePowerup,
                                        "../Assets/Sprites/Collectibles/Time_Powerup.png");

                timePowerup->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if (tile == 36) // Spawner
            {
                const auto press = new PressMachine(this, mRenderer);
                press->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if (
                tile == 0 || tile == 8 ||
                tile == 4 || tile == 5 || tile == 11 || tile == 12 ||
                tile == 18 || tile == 19 || tile == 26 || tile == 33 || tile ==
                40
            )
            {
                if (const auto it = tileMap.find(tile);
                    it != tileMap.end())
                {
                    // Create an enemy block actor
                    const auto enemyBlock = new EnemyBlock(this, it->second);
                    enemyBlock->SetPosition(
                        Vector2(x * TILE_SIZE, y * TILE_SIZE));
                }
            }
            else
            {
                if (const auto it = tileMap.find(tile);
                    it != tileMap.end())
                {
                    // Create a block actor
                    const auto block = new Block(this, it->second);
                    block->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                }
            }
        }
    }
}

void Game::BuildSecondLevel(int **levelData, int width, int height)
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

            if (tile == 17) // Mario
            {
                mRobotPlane = new RobotPlane(this);
                mRobotPlane->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if (tile == 24)
            {
                auto *press = new PressMachine(this, mRenderer);
                press->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
            }
            else if (
                tile == 0 || tile == 8 ||
                tile == 6 || tile == 7 || tile == 13 || tile == 14 || tile == 15
                ||
                tile == 20 || tile == 21 || tile == 22 || tile == 27 || tile ==
                28 ||
                tile == 29 || tile == 35 || tile == 36 || tile == 42 || tile ==
                43 ||
                tile == 49 || tile == 50 || tile == 56 || tile == 57 || tile ==
                64)
            {
                auto it = tileMap.find(tile);
                if (it != tileMap.end())
                {
                    EnemyBlock *enemyBlock = new EnemyBlock(this, it->second);
                    enemyBlock->SetPosition(
                        Vector2(x * TILE_SIZE, y * TILE_SIZE));
                }
            }
            else // Blocks
            {
                auto it = tileMap.find(tile);
                if (it != tileMap.end())
                {
                    // Create a block actor
                    Block *block = new Block(this, it->second);
                    block->SetPosition(Vector2(x * TILE_SIZE, y * TILE_SIZE));
                }
            }
        }
    }
}

int **Game::ReadLevelData(const std::string &fileName,
                          const int width,
                          const int height)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        SDL_Log("Failed to load paths: %s", fileName.c_str());
        return nullptr;
    }

    // Create a 2D array of size width and height to store the level data
    const auto levelData = new int *[height];
    for (int i = 0; i < height; ++i) { levelData[i] = new int[width]; }

    // Read the file line by line
    int row = 0;

    std::string line;
    while (!file.eof())
    {
        std::getline(file, line);
        if (!line.empty())
        {
            auto tiles = CSVHelper::Split(line);

            if (tiles.size() != width)
            {
                SDL_Log("Invalid level data");
                return nullptr;
            }

            for (int i = 0; i < width; ++i) { levelData[row][i] = tiles[i]; }
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
            {
                Quit();
                break;
            }
            case SDL_KEYDOWN:
            {
                const bool isPlayingScene = (mGameScene == GameScene::TutorialLevel||
                                            mGameScene == GameScene::Level1 ||
                                            mGameScene == GameScene::Level2);

                if (event.key.keysym.sym == SDLK_RETURN && isPlayingScene &&
                    mGamePlayState == GamePlayState::Playing) { TogglePause(); }

                else if (!mUIStack.empty())
                {
                    mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
                }

                if (mGamePlayState == GamePlayState::Playing)
                {
                    HandleKeyPressActors(event.key.keysym.sym,
                                         event.key.repeat == 0);
                }
                break;
            }
            default: break;
        }
    }

    ProcessInputActors();
}

void Game::ProcessInputActors() const
{
    if (mGamePlayState == GamePlayState::Playing)
    {
        // Get actors on camera
        std::vector<Actor *> actorsOnCamera =
                mSpatialHashing->QueryOnCamera(mCameraPos,
                                               mWindowWidth,
                                               mWindowHeight,
                                               TILE_SIZE);

        const Uint8 *state = SDL_GetKeyboardState(nullptr);

        bool isPlayerOnCamera = false;
        for (const auto &actor: actorsOnCamera)
        {
            actor->ProcessInput(state);

            if (actor == mPlayer || actor == mRobotPlane)
            {
                isPlayerOnCamera = true;
            }
        }

        // If Player is not on camera, process input for him
        if (!isPlayerOnCamera && mPlayer) { mPlayer->ProcessInput(state); }
        else if (!isPlayerOnCamera && mRobotPlane)
        {
            mRobotPlane->ProcessInput(state);
        }
    }
}

void Game::HandleKeyPressActors(const int key, const bool isPressed) const
{
    if (mGamePlayState == GamePlayState::Playing)
    {
        // Get actors on camera
        std::vector<Actor *> actorsOnCamera =
                mSpatialHashing->QueryOnCamera(mCameraPos,
                                               mWindowWidth,
                                               mWindowHeight);

        // Handle key press for actors
        bool isPlayerOnCamera = false;
        for (const auto &actor: actorsOnCamera)
        {
            actor->HandleKeyPress(key, isPressed);

            if (actor == mPlayer || actor == mRobotPlane)
            {
                isPlayerOnCamera = true;
            }
        }

        // If Player is not on camera, handle key press for him
        if (!isPlayerOnCamera && mPlayer)
        {
            mPlayer->HandleKeyPress(key, isPressed);
        }
        else if (!isPlayerOnCamera && mRobotPlane)
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
            mAudio->PauseSound(mMusicHandle);
            new PauseMenu(this);
        }
        else if (mGamePlayState == GamePlayState::Paused)
        {
            mGamePlayState = GamePlayState::Playing;
            mAudio->ResumeSound(mMusicHandle);
        }
    }
}

void Game::ToggleTutorial()
{
    if (mGameScene == GameScene::TutorialLevel
        && mSceneAttempts[GameScene::TutorialLevel] == 1
    )
    {

        if (mGamePlayState == GamePlayState::Playing)
        {
            mGamePlayState = GamePlayState::ShowingTutorial;
            mAudio->PauseSound(mMusicHandle);
            new TutorialGuide(this);
        }
        else if (mGamePlayState == GamePlayState::ShowingTutorial)
        {
            mGamePlayState = GamePlayState::Playing;
            mAudio->ResumeSound(mMusicHandle);
        }
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16)) {};

    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    if (deltaTime > 0.05f) { deltaTime = 0.05f; }

    // Slows time down depending on time powerup.
    // By default, mSlowingFactor = 1
    deltaTime /= mSlowingFactor;

    mTicksCount = SDL_GetTicks();

    if (mGamePlayState != GamePlayState::Paused
        && mGamePlayState != GamePlayState::ShowingTutorial
        && mGamePlayState != GamePlayState::GameOver)
    {
        // Reinsert all actors and pending actors
        UpdateActors(deltaTime);
    }

    // Reinsert audio system
    mAudio->Update(deltaTime);

    // Reinsert UI screens
    for (const auto &ui: mUIStack)
    {
        if (ui->GetState() == UIScreen::UIState::Active)
        {
            ui->Update(deltaTime);
        }
    }

    // Delete any UIElements that are closed
    auto iter = mUIStack.begin();
    while (iter != mUIStack.end())
    {
        if ((*iter)->GetState() == UIScreen::UIState::Closing)
        {
            delete *iter;
            iter = mUIStack.erase(iter);
        }
        else { ++iter; }
    }

    // ---------------------
    // Game Specific Updates
    // ---------------------
    UpdateCamera(deltaTime);
    UpdateSceneManager(deltaTime);


    if (mGameScene != GameScene::MainMenu && mGamePlayState ==
        GamePlayState::Playing)
    {
        UpdateLevelTime(deltaTime);
        if (mPlayer)
        {
            const float playerX = mPlayer->GetPosition().x;

            if (constexpr float levelLimitX = LEVEL_WIDTH * TILE_SIZE;
                mGameScene == GameScene::Level1 && playerX >= levelLimitX)
            {
                mPlayer->SetState(ActorState::Destroy);
                mPlayer = nullptr;
                this->GetAudio()->StopAllSounds();

                SetGameScene(GameScene::GameWinScreen, 0.5);
            }
            if (constexpr float levelLimitX = TUTORIAL_LEVEL_WIDTH * TILE_SIZE;
                mGameScene == GameScene::TutorialLevel && playerX >= levelLimitX)
            {
                mPlayer->SetState(ActorState::Destroy);
                mPlayer = nullptr;
                this->GetAudio()->StopAllSounds();

                SetGameScene(GameScene::Level2, 0.5);
            }
        }
        else if (mRobotPlane)
        {
            const float playerX = mRobotPlane->GetPosition().x;
            if (constexpr float levelLimitX = LEVEL_WIDTH * TILE_SIZE;
                mGameScene == GameScene::Level2 && playerX >= levelLimitX)
            {
                mRobotPlane->SetState(ActorState::Destroy);
                mRobotPlane = nullptr;
                this->GetAudio()->StopAllSounds();
                SetGameScene(GameScene::Level1, 0.5);
            }
        }
    }
}

void Game::UpdateSceneManager(const float deltaTime)
{
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

    else if (mSceneManagerState == SceneManagerState::Active)
    {
        mSceneManagerTimer -= deltaTime;

        if (mSceneManagerTimer <= 0.0f)
        {
            mSceneManagerState = SceneManagerState::None;
        }
    }
}

void Game::UpdateLevelTime(const float deltaTime)
{
    mGameTimer += deltaTime;

    if (mGameTimer >= 1.0f)
    {
        mGameTimer = 0.0f;
        mGameTimeLimit--;

        if (mGameTimeLimit <= 0 && mPlayer)
        {
            //mPlayer->Kill();
        }
    }
}

void Game::UpdateCamera(const float deltaTime)
{
    if (!mPlayer && !mRobotPlane) return;

    if (mGameScene == GameScene::Level1 || mGameScene == GameScene::TutorialLevel )
    {
        if (mGamePlayState == GamePlayState::Playing)
        {
            int levelWidth = mGameScene == GameScene::Level1
                ? LEVEL_WIDTH
                : TUTORIAL_LEVEL_WIDTH;

            const float horizontalPos = Math::Max(
                mPlayer->GetPosition().x - mWindowWidth / 4,
                mCameraPos.x + deltaTime * CAMERA_X_SPEED);
            if (const float maxCameraPos =
                        (levelWidth * TILE_SIZE) - mWindowWidth;
                horizontalPos > maxCameraPos) { mCameraPos.x = maxCameraPos; }
            else { mCameraPos.x = horizontalPos; }
        }
        //horizontalCameraPos = mMario->GetPosition().x - (mWindowWidth / 2.0f);
    }
    else if (mGameScene == GameScene::Level2)
    {
        float horizontalCameraPos =
                mRobotPlane->GetPosition().x - (mWindowWidth / 2.0f);
        if (horizontalCameraPos > mCameraPos.x)
        {
            // Limit camera to the right side of the level
            float maxCameraPos = (LEVEL_WIDTH * TILE_SIZE) - mWindowWidth;
            horizontalCameraPos = Math::Clamp(horizontalCameraPos, 0.0f,
                                              maxCameraPos);

            mCameraPos.x = horizontalCameraPos;
        }
    }
}

void Game::UpdateActors(const float deltaTime)
{
    // Get actors on camera
    std::vector<Actor *> actorsOnCamera =
            mSpatialHashing->QueryOnCamera(mCameraPos,
                                           mWindowWidth,
                                           mWindowHeight,
                                           TILE_SIZE * 5);

    bool isPlayerOnCamera = false;
    for (const auto &actor: actorsOnCamera)
    {
        actor->Update(deltaTime);
        if ((mPlayer && actor == mPlayer) || (
                mRobotPlane && actor == mRobotPlane))
        {
            isPlayerOnCamera = true;
        }
    }

    // If Player is not on camera, reset camera position
    if (!isPlayerOnCamera && mPlayer) { mPlayer->Update(deltaTime); }
    else if (!isPlayerOnCamera && mRobotPlane)
    {
        mRobotPlane->Update(deltaTime);
    }

    for (const auto &actor: actorsOnCamera)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            delete actor;
            if (actor == mPlayer) { mPlayer = nullptr; }
            else if (actor == mRobotPlane) { mRobotPlane = nullptr; }
        }
    }
}

void Game::AddActor(Actor *actor) const { mSpatialHashing->Insert(actor); }

void Game::RemoveActor(Actor *actor) const { mSpatialHashing->Remove(actor); }
void Game::Reinsert(Actor *actor) const { mSpatialHashing->Reinsert(actor); }

std::vector<Actor *>
Game::GetNearbyActors(const Vector2 &position, const int range) const
{
    return mSpatialHashing->Query(position, range);
}

std::vector<AABBColliderComponent *>
Game::GetNearbyColliders(const Vector2 &position, const int range) const
{
    return mSpatialHashing->QueryColliders(position, range);
}

void Game::GenerateOutput()
{
    // Clear frame with background color
    SDL_SetRenderDrawColor(mRenderer, mBackgroundColor.x, mBackgroundColor.y,
                           mBackgroundColor.z, 255);

    // Clear back buffer
    SDL_RenderClear(mRenderer);

    if (mGameScene == GameScene::TutorialLevel || mGameScene == GameScene::Level1 || mGameScene == GameScene::Level2)
    {
        DrawParallaxBackground(mRenderer, mCameraPos);
    }

    // Get actors on camera
    const std::vector<Actor *> actorsOnCamera =
            mSpatialHashing->QueryOnCamera(mCameraPos,
                                           mWindowWidth,
                                           mWindowHeight,
                                           TILE_SIZE * 5);

    // Get list of drawables in draw order
    std::vector<DrawComponent *> drawables;

    for (const auto &actor: actorsOnCamera)
    {
        if (auto drawable = actor->GetComponent<DrawComponent>();
            drawable && drawable->IsVisible())
        {
            drawables.emplace_back(drawable);
        }
    }

    // Sort drawables by draw order
    std::sort(drawables.begin(),
              drawables.end(),
              [](const DrawComponent *a, const DrawComponent *b)
              {
                  return a->GetDrawOrder() < b->GetDrawOrder();
              }
    );

    // Draw all drawables
    for (const auto &drawable: drawables)
    {
        drawable->Draw(mRenderer, mModColor);
    }

    for (const auto &ui: mUIStack) { ui->Draw(mRenderer); }

    // Draw all UI screens
    for (const auto &ui: mUIStack) { ui->Draw(mRenderer); }

    // Crossfading transition
    if (mSceneManagerState == SceneManagerState::Entering || mSceneManagerState
        == SceneManagerState::Active)
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
        const SDL_Rect blackRect = {0, 0, mWindowWidth, mWindowHeight};
        SDL_RenderFillRect(mRenderer, &blackRect);

        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_NONE);
    }

    // Swap front buffer and back buffer
    SDL_RenderPresent(mRenderer);
}

void Game::SetBackgroundImage(const std::string &imagePath,
                              const Vector2 &position,
                              const Vector2 &size)
{
    if (mBackgroundTexture)
    {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }

    // Load background texture
    mBackgroundTexture = LoadTexture(imagePath);
    if (!mBackgroundTexture)
    {
        SDL_Log("Failed to load background texture: %s", imagePath.c_str());
    }

    // Set background position
    mBackgroundPosition.Set(position.x, position.y);

    // Set background size
    mBackgroundSize.Set(size.x, size.y);
}

SDL_Texture *Game::LoadTexture(const std::string &texturePath) const
{
    SDL_Surface *surface = IMG_Load(texturePath.c_str());

    if (!surface)
    {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return nullptr;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    if (!texture)
    {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }

    return texture;
}

UIFont *Game::LoadFont(const std::string &fileName)
{
    if (const auto iter = mFonts.find(fileName);
        iter != mFonts.end()) { return iter->second; }

    const auto newFont = new UIFont(mRenderer);
    if (newFont->Load(fileName))
    {
        mFonts[fileName] = newFont;
        return newFont;
    }

    newFont->Unload();
    delete newFont;
    SDL_Log("Failed to load font: %s", fileName.c_str());
    return nullptr;
}

void Game::DrawParallaxBackground(SDL_Renderer *renderer,
                                  const Vector2 &cameraPos) const
{
    SDL_Texture *fixedBg1 = mCurrentParallax->bg1;
    SDL_Texture *fixedBg2 = mCurrentParallax->bg2;

    int texW1, texH1;
    SDL_QueryTexture(fixedBg1, nullptr, nullptr, &texW1, &texH1);
    const SDL_Rect dst1 = {0, 0, texW1, texH1};
    SDL_RenderCopy(renderer, fixedBg1, nullptr, &dst1);

    int texW2, texH2;
    SDL_QueryTexture(fixedBg2, nullptr, nullptr, &texW2, &texH2);
    const SDL_Rect dst2 = {0, 0, texW2, texH2};
    SDL_RenderCopy(renderer, fixedBg2, nullptr, &dst2);

    struct Layer
    {
        SDL_Texture *texture;
        float parallaxFactor;
    };

    std::vector<Layer> layers = {
            {mCurrentParallax->layer3, 0.1f},
            {mCurrentParallax->layer4, 0.3f},
            {mCurrentParallax->layer5, 0.5f}
            };

    const int screenWidth = GetWindowWidth();

    for (const auto &[texture, parallaxFactor]: layers)
    {
        int texW, texH;
        SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);

        float scrollX = fmod(cameraPos.x * parallaxFactor,
                             static_cast<float>(texW));
        if (scrollX < 0) scrollX += texW;

        const int numTiles = screenWidth / texW + 2;

        for (int i = 0; i < numTiles; ++i)
        {
            SDL_Rect dst = {
                    static_cast<int>(-scrollX + i * texW),
                    0,
                    texW,
                    texH
                    };

            SDL_RenderCopy(renderer, texture, nullptr, &dst);
        }
    }
}

void Game::SetHUDPowerupUsable(bool isUsable) const
{
    mHUD->SetPowerupUsable(isUsable);
}

void Game::UnloadScene()
{
    // Delete actors
    delete mSpatialHashing;

    // Delete UI screens
    for (const auto &ui: mUIStack) { delete ui; }
    mUIStack.clear();

    // Delete background texture
    if (mBackgroundTexture)
    {
        SDL_DestroyTexture(mBackgroundTexture);
        mBackgroundTexture = nullptr;
    }
}

void Game::Shutdown()
{
    UnloadScene();

    for (const auto &font: mFonts)
    {
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

    for (auto &[scene, parallax]: mParallaxCache)
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
