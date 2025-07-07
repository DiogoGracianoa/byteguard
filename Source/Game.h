// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <SDL.h>
#include <unordered_map>
#include <vector>
#include "AudioSystem.h"
#include "HUD.h"
#include "Math.h"

class Game
{
public:
    static constexpr int LEVEL_WIDTH = 200;
    static constexpr int TUTORIAL_LEVEL_WIDTH = 100;
    static constexpr int LEVEL_HEIGHT = 15;
    static constexpr int TILE_SIZE = 40;
    static constexpr int SPAWN_DISTANCE = 700;
    static constexpr int TRANSITION_TIME = 1;
    static constexpr float CAMERA_X_SPEED = 250.0f;

    struct ParallaxSet
    {
        SDL_Texture *bg1;
        SDL_Texture *bg2;
        SDL_Texture *layer3;
        SDL_Texture *layer4;
        SDL_Texture *layer5;
    };

    enum class GameScene
    {
        MainMenu,
        StoryScreen,
        TutorialLevel,
        EasyLevel,
        Level1,
        Level2,
        GameWinScreen
    };

    enum class SceneManagerState
    {
        None,
        Entering,
        Active,
        Exiting
    };

    enum class GamePlayState
    {
        Playing,
        Paused,
        GameOver,
        LevelComplete,
        Leaving,
        ShowingTutorial,
    };

    Game(int windowWidth, int windowHeight);

    bool Initialize();

    void RunLoop();

    void Shutdown();

    void Quit() { mIsRunning = false; }

    // Actor functions
    void UpdateActors(float deltaTime);

    void AddActor(class Actor *actor) const;

    void RemoveActor(class Actor *actor) const;

    void ProcessInputActors() const;

    void HandleKeyPressActors(int key, bool isPressed) const;

    // Level functions
    void LoadMainMenu();

    void LoadLevel(const std::string &levelName,
                   int levelWidth,
                   int levelHeight,
                   int level);

    std::vector<Actor *> GetNearbyActors(const Vector2 &position,
                                         int range = 1) const;

    std::vector<class AABBColliderComponent *> GetNearbyColliders(
        const Vector2 &position,
        int range = 2) const;

    void Reinsert(Actor *actor) const;

    // Camera functions
    Vector2 &GetCameraPos() { return mCameraPos; };
    void SetCameraPos(const Vector2 &position) { mCameraPos = position; };

    // Audio functions
    class AudioSystem *GetAudio() const { return mAudio; }

    // UI functions
    void PushUI(class UIScreen *screen) { mUIStack.emplace_back(screen); }
    const std::vector<class UIScreen *> &GetUIStack() { return mUIStack; }

    // Window functions
    int GetWindowWidth() const { return mWindowWidth; }
    int GetWindowHeight() const { return mWindowHeight; }

    // Loading functions
    class UIFont *LoadFont(const std::string &fileName);

    SDL_Texture *LoadTexture(const std::string &texturePath) const;

    void SetGameScene(GameScene scene, float transitionTime = .0f);

    void ResetGameScene(float transitionTime = .0f);

    void UnloadScene();

    void SetBackgroundImage(const std::string &imagePath,
                            const Vector2 &position = Vector2::Zero,
                            const Vector2 &size = Vector2::Zero);

    void TogglePause();

    void ToggleTutorial();

    // Game-specific
    const class Player *GetPlayer() const { return mPlayer; }

    void SetGamePlayState(GamePlayState state) { mGamePlayState = state; }
    GamePlayState GetGamePlayState() const { return mGamePlayState; }

    // Time powerup methods
    void SetSlowingFactor(const float factor) { mSlowingFactor = factor; }
    void ResetSlowingFactor() { mSlowingFactor = 1.0f; }

    // Set HUD powerup
    void SetHUDPowerupUsable(bool isUsable) const;

private:
    void ProcessInput();

    void UpdateGame();

    void UpdateCamera(float deltaTime);

    void GenerateOutput();

    // Scene Manager
    void UpdateSceneManager(float deltaTime);

    void ChangeScene();

    SceneManagerState mSceneManagerState;
    float mSceneManagerTimer;

    void DrawParallaxBackground(SDL_Renderer *renderer,
                                const Vector2 &cameraPos) const;

    // Load the level from a CSV file as a 2D array
    static int **ReadLevelData(const std::string &fileName,
                               int width,
                               int height);

    void BuildTutorialLevel(int **levelData, int width, int height);

    void BuildIndustrialLevel(int **levelData, int width, int height);

    void BuildRobotPlaneLevel(int **levelData, int width, int height);

    // Spatial Hashing for collision detection
    class SpatialHashing *mSpatialHashing;

    // All the UI elements
    std::vector<class UIScreen *> mUIStack;
    std::unordered_map<std::string, class UIFont *> mFonts;

    // SDL stuff
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    AudioSystem *mAudio;

    // Window properties
    int mWindowWidth;
    int mWindowHeight;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track actors state
    bool mIsRunning;
    GamePlayState mGamePlayState;

    // Track level state
    GameScene mGameScene;
    GameScene mNextScene;

    // Background and camera
    Vector3 mBackgroundColor;
    Vector3 mModColor;
    Vector2 mCameraPos;

    // Game-specific
    class Player *mPlayer;
    class RobotPlane *mRobotPlane;
    class HUD *mHUD;
    SoundHandle mMusicHandle;

    SDL_Texture *mBackgroundTexture;
    Vector2 mBackgroundSize;
    Vector2 mBackgroundPosition;

    //Parallax camera
    std::map<GameScene, ParallaxSet> mParallaxCache;
    ParallaxSet *mCurrentParallax = nullptr;

    std::unordered_map<GameScene, int> mSceneAttempts;

    // time powerup slowing factor
    float mSlowingFactor;
};
