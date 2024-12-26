#include <vector>

#include "raylib.h"

#define GRAVITY 600
#define PLAYER_JUMP_SPD 400.0f
#define PLAYER_HOR_SPD 400.0f

typedef struct {
    Vector2 position;
    float width;
    float height;
    float speed;
    bool canJump;
} Player;

typedef struct {
    Rectangle rect;
    Color colour;
} Item;

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

    Player player;
    player.position = (Vector2) {
        75.f,
        600.f
    };
    player.width = 50.f;
    player.height = 70.f;
    player.speed = 0;
    player.canJump = false;

    std::vector<Item> items = {
        {
            // Ground
            { GetScreenWidth() / 2.f, (float)GetScreenHeight(), (float)GetScreenWidth(), 50.f },
            LIME
        },
        {
            // Platform
            { 200.f, 600.f, 100.f, 20.f },
            LIGHTGRAY
        },
        {
            // Platform
            { 400.f, 500.f, 100.f, 20.f },
            LIGHTGRAY            
        },
        {
            // Platform
            { 600.f, 400.f, 100.f, 20.f },
            LIGHTGRAY
        },
        {
            // Platform
            { 800.f, 300.f, 100.f, 20.f },
            LIGHTGRAY
        },
        {
            // Platform
            { 1000.f, 200.f, 100.f, 20.f },
            LIGHTGRAY
        },
    };

    while (!WindowShouldClose()) {

        // Update
        updatePlayerPosition(player, items);
        
        // Draw
        BeginDrawing();
            ClearBackground(BLACK);

            // Items
            for (auto i : items) {
                Rectangle itemRect = {
                    i.rect.x - (i.rect.width / 2),
                    i.rect.y - i.rect.height,
                    i.rect.width,
                    i.rect.height
                };
                DrawRectangleRec(itemRect, i.colour);
            }

            // Player
            Rectangle playerRect = {
                player.position.x - (player.width / 2),
                player.position.y - player.height,
                player.width,
                player.height
            };
            DrawRectangleRec(playerRect, GOLD);
            // DrawRectangle(player.x - 10, player.y - 30, player.width / 5, player.height + 10, BLUE);
            // DrawRectangle(player.x + player.width, player.y - 30, player.width / 5, player.height + 10, BLUE);
            // DrawRectangle(player.x, player.y, player.width, 10.f, GOLD);

            DrawText("Arrow keys for moving - 'r' to restart", 30, 30, 44, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
