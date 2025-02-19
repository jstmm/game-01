#pragma once

#include "raylib.h"

#include <vector>

#define GRAVITY 600
#define PLAYER_JUMP_SPD 400.0f
#define PLAYER_HOR_SPD 400.0f
#define SCREEN_WIDTH 1280.0f
#define SCREEN_HEIGHT 720.0f

enum class State { Idle, MovingLeft, MovingRight };

enum class Mode { Play, Edit };

typedef struct {
    Rectangle rec;
    Vector2 origin;
    State state;
    float speed;
    bool can_jump;
    int current_image;
    int time_in_idle_state;
    int time_in_walking_state;
    unsigned int score;
} Player;

typedef struct {
    Rectangle rec;
    Vector2 origin;
} Platform;

typedef struct {
    Rectangle rec;
    Vector2 origin;
    bool is_collected;
} Collectible;

typedef struct {
    Player player;
    std::vector<Platform> platforms;
    std::vector<Collectible> collectibles;
    Collectible* selected_collectible;
} Scene;

typedef struct {
    Texture2D sprite_sheet;
    Rectangle src_platform;
    Rectangle src_player;
    Rectangle src_collectible;
    Texture2D icons_sprite_sheet;
} SpriteCollection;

typedef struct {
    Texture2D* texture;
    Rectangle source;
    Vector2 origin;
    Rectangle destination;
    bool is_selected;
} Button;

typedef struct {
    std::vector<Button> buttons;
    int button_selected;
} Editor;

typedef struct {
    Mode current_mode;
    Editor editor;
    bool is_running = true;
    bool show_origins = false;
} Game;
