#include <vector>
#include <iostream>

#include "raylib.h"

#define GRAVITY 600
#define PLAYER_JUMP_SPD 400.0f
#define PLAYER_HOR_SPD 400.0f

enum class State {
    Idle,
    MovingLeft,
    MovingRight
};

typedef struct {
    Vector2 position;
    float width;
    float height;
    float speed;
    bool canJump;
    State state;
} Player;

typedef struct {
    Rectangle rect;
    Color colour;
} Item;

bool areColoursEqual(Color c1, Color c2) {
    return ((c1.r == c2.r) && (c1.g == c2.g) && (c1.b == c2.b) && (c1.a == c2.a));
}

void updatePlayerPosition(Player& player, const std::vector<Item>& items)
{
    const float deltaTime = GetFrameTime();
    const float minX = player.width / 2;
    const float maxX = (float) GetScreenWidth() - (player.width / 2);

    bool hitObstacle = false;

    if (IsKeyPressed(KEY_R)) {
        player.position.x = 75.f;
        player.position.y = 600.f;
        player.speed = 0;
        player.canJump = false;
    }

    if (IsKeyDown(KEY_LEFT)) {
        player.position.x -= PLAYER_HOR_SPD * deltaTime;
    }
    
    if (IsKeyDown(KEY_RIGHT)) {
        player.position.x += PLAYER_HOR_SPD * deltaTime;
    }
    
    if (IsKeyPressed(KEY_SPACE) && player.canJump) {
        player.speed = -PLAYER_JUMP_SPD;
        player.canJump = false;
    }

    for (Item i : items) {
        if (
            (i.rect.x - (i.rect.width/2) <= player.position.x + (player.width/2)) &&
            (player.position.x - (player.width/2) <= i.rect.x + (i.rect.width/2)) &&
            (player.position.y <= i.rect.y - i.rect.height) &&
            (player.position.y + (player.speed * deltaTime) >= i.rect.y - i.rect.height)
        ) {
                hitObstacle = true;
                player.speed = 0.0f;
                player.position.y = i.rect.y - i.rect.height;        
                break;
        }
    }

    if (!hitObstacle) {
        player.position.y += player.speed * deltaTime;
        player.speed += GRAVITY * deltaTime;
        player.canJump = false;
    } else {
        player.canJump = true;
    }

    if (player.position.x <= minX) player.position.x = minX;
    if (player.position.x >= maxX) player.position.x = maxX;
}

int main()
{
    InitWindow(1280, 720, "Game 01");
    SetTargetFPS(60);
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
  
    Texture2D texture = LoadTexture("resources/monkeylad_further.png");

    float platformWidth = 47, platformHeight = 8;
    Rectangle srcPlatform = { 448, 32, 47, 8 };

    float playerWidth = 16;
    Rectangle srcPlayer = { 448, 208, 16, 24 };
    std::vector<Vector2> monkeys = {
        { 448, 208 },
        { 464, 208 },
        { 480, 208 },
        { 496, 208 },
        { 512, 208 },
        { 528, 208 },
        { 544, 208 },
    };

    std::vector<Item> items = {
        // Ground
        {
            { screenWidth / 2.f, (float)screenHeight, (float)screenWidth, 50.f },
            LIME
        },
        // Platforms
        {
            { 200.f, 600.f, 100.f, 20.f },
            LIGHTGRAY
        },
        {
            { 400.f, 500.f, 100.f, 20.f },
            LIGHTGRAY            
        },
        {
            { 600.f, 400.f, 100.f, 20.f },
            LIGHTGRAY
        },
        {
            { 800.f, 300.f, 100.f, 20.f },
            LIGHTGRAY
        },
        {
            { 1000.f, 200.f, 100.f, 20.f },
            LIGHTGRAY
        },
    };
    Vector2 originPlatform = { 100.f / 2, 20.f };
    
    short scale = 3;
    Player player;
    player.position = (Vector2) {
        75.f,
        600.f
    };
    player.width = srcPlayer.width * scale;
    player.height = srcPlayer.height * scale;
    player.speed = 0;
    player.canJump = false;
    player.state = State::Idle;
    Vector2 originPlayer = { player.width / 2, player.height };

    int currentImage = 0;
    int delay = 120;

    while (!WindowShouldClose()) {

        // Update
        updatePlayerPosition(player, items);

        if (IsKeyDown(KEY_RIGHT)) {
            player.state = State::MovingRight;
        }
        else if (IsKeyDown(KEY_LEFT)) {
            player.state = State::MovingLeft;
        }
        else if (player.state != State::Idle) {
            if (delay > 0) {
                // Small delay before returning to Idle state
                --delay;
            }
            else {
                player.state = State::Idle;
                delay = 120;
            }
        }

        switch (player.state) {
            case State::Idle:
                currentImage = 0;
                srcPlayer.width = playerWidth;
                break;

            case State::MovingLeft:
                currentImage = 2;
                srcPlayer.width = -playerWidth;
                break;

            case State::MovingRight:
                currentImage = 2;
                srcPlayer.width = playerWidth;
                break;
        }
        
        srcPlayer.x = monkeys.at(currentImage).x;
        srcPlayer.y = monkeys.at(currentImage).y;
        
        // Draw
        BeginDrawing();
            ClearBackground(SKYBLUE);

            // Environment
            for (auto item : items) {
                if (areColoursEqual(item.colour, LIME)) {
                    Rectangle rect = {
                        item.rect.x - (item.rect.width / 2),
                        item.rect.y - item.rect.height,
                        item.rect.width,
                        item.rect.height
                    };
                    DrawRectangleRec(rect, item.colour);
                }
                else {
                    DrawTexturePro(texture, srcPlatform, item.rect, originPlatform, 0, WHITE);
                }
            }

            // Player
            Rectangle destRect = {
                player.position.x,
                player.position.y,
                player.width,
                player.height
            };
            DrawTexturePro(texture, srcPlayer, destRect, originPlayer, 0, WHITE);

            DrawText("Arrow keys for moving\nSpace key to jump\n'r' to restart", 30, 30, 44, WHITE);
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();

    return 0;
}
