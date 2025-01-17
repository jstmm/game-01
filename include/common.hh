#pragma once

#define GRAVITY 600
#define PLAYER_JUMP_SPD 400.0f
#define PLAYER_HOR_SPD 400.0f
#define SCREEN_WIDTH 1280.0f
#define SCREEN_HEIGHT 720.0f

#include "raylib.h"

enum class State
{
    Idle,
    MovingLeft,
    MovingRight
};

enum class Mode
{
    Play,
    Edit
};

typedef struct
{
    float width;
    float height;
    Vector2 origin;
    Vector2 position;
    State state;
    float speed;
    bool canJump;
    int currentImage;
    int timeInIdleState;
    int timeInWalkingState;
    unsigned int score;
} Player;

typedef struct
{
    Rectangle rec;
} Platform;

typedef struct
{
    Rectangle rec;
    Vector2 origin;
    bool isCollected;
} Collectible;

typedef struct
{
    Player player;
    std::vector<Platform> platforms;
    std::vector<Collectible> collectibles;
    Collectible *selectedCollectible;
} Scene;

typedef struct
{
    Texture2D spriteSheet;
    Rectangle srcPlatform;
    Rectangle srcPlayer;
    Rectangle srcCollectible;
    Texture2D iconsSpriteSheet;
} SpriteCollection;

typedef struct {
    Texture2D *texture;
    Rectangle source;
    Vector2 origin;
    Rectangle destination;
    bool isSelected;
} Button;

typedef struct {
    std::vector<Button> buttons;
    int buttonSelected;
} Editor;

typedef struct
{
    Mode currentMode;
    Editor editor;
} Game;

Scene scene;
SpriteCollection spriteCollection;
Game game;
