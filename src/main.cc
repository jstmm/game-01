#include <vector>
#include <iostream>

#include "raylib.h"

#define GRAVITY 600
#define PLAYER_JUMP_SPD 400.0f
#define PLAYER_HOR_SPD 400.0f
#define SCREEN_WIDTH 1280.0f
#define SCREEN_HEIGHT 720.0f

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
    Vector2 origin;
    int currentImage;
    int timeInIdleState;
    int timeInWalkingState;
} Player;

typedef struct {
    Rectangle rect;
    Color colour;
} Item;

bool areColoursEqual(Color c1, Color c2) {
    return ((c1.r == c2.r) && (c1.g == c2.g) && (c1.b == c2.b) && (c1.a == c2.a));
}

void updateStates(Player& player) {
    if (IsKeyPressed(KEY_R)) {
        player.position.x = 75.f;
        player.position.y = 600.f;
        player.speed = 0;
        player.canJump = false;
        player.timeInIdleState = 0;
        player.timeInWalkingState = 0;
        return;
    }

    if (IsKeyPressed(KEY_SPACE) && player.canJump) {
        player.speed = -PLAYER_JUMP_SPD;
        player.canJump = false;
    }

    if (!player.canJump) {
        player.timeInIdleState = 0;
        player.timeInWalkingState = 0;
    }

    if (IsKeyDown(KEY_LEFT)) {
        player.state = State::MovingLeft;
        if (player.canJump) {
            ++player.timeInWalkingState;
            player.timeInIdleState = 0;
        }
    }
    else if (IsKeyDown(KEY_RIGHT)) {
        player.state = State::MovingRight;
        if (player.canJump) {
            ++player.timeInWalkingState;
            player.timeInIdleState = 0;
        }
    }
    else {
        player.state = State::Idle;
        if (player.canJump) {
            ++player.timeInIdleState;
            player.timeInWalkingState = 0;
        }
    }
}

void updatePlayerPosition(Player& player, const std::vector<Item>& items)
{
    const float deltaTime = GetFrameTime();
    const float minX = player.width / 2;
    const float maxX = SCREEN_WIDTH - (player.width / 2);

    bool hitObstacle = false;

    if (player.state == State::MovingLeft) {
        player.position.x -= PLAYER_HOR_SPD * deltaTime;
    }
    
    if (player.state == State::MovingRight) {
        player.position.x += PLAYER_HOR_SPD * deltaTime;
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

int imageRefForTimeInWalkingState(const int time)
{
    std::vector<int> imageRefs = {2, 3, 4, 5};
    int timePerImage = 6;

    int duration = imageRefs.size() * timePerImage;
    int result;

    for (int i = 0; i < imageRefs.size(); ++i) {
        if ((time % duration) < (timePerImage * (i + 1))) {
            result = imageRefs.at(i);
            break;
        }
    }
    return result;
}

void updatePlayerImage(Player& player, Rectangle& srcPlayer, int playerWidth) {
    if (!player.canJump) {
        player.currentImage = 3;
        srcPlayer.width = (player.state == State::MovingLeft) ? -playerWidth :
                          (player.state == State::MovingRight) ? playerWidth :
                          srcPlayer.width;
        return;
    }

    if (player.state == State::MovingLeft) {
        player.currentImage = imageRefForTimeInWalkingState(player.timeInWalkingState);
        srcPlayer.width = -playerWidth;
        return;
    }

    if (player.state == State::MovingRight) {
        player.currentImage = imageRefForTimeInWalkingState(player.timeInWalkingState);
        srcPlayer.width = playerWidth;
        return;
    }

    if (player.state == State::Idle) {
        if (player.timeInIdleState < 120) {
            player.currentImage = 1;
        }
        else {
            player.currentImage = 0;
        }
    }
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game 01");
    SetTargetFPS(60);
  
    // Import the sprite sheet
    Texture2D spriteSheet = LoadTexture("resources/monkeylad_further.png");
    float platformWidth = 47, platformHeight = 8;
    Rectangle srcPlatform = { 448, 33, 47, 8 };
    float playerWidth = 16;
    Rectangle srcPlayer = { 448, 208, 16, 24 };
    std::vector<Vector2> monkeys = {
        { 448, 208 },
        { 464, 208 },
        { 480, 208 },
        { 496, 208 },
        { 512, 208 },
        { 528, 208 },
    };

    // Initialise entities
    std::vector<Item> items = {
        {
            { SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50.f },
            LIME
        },
        {
            { 200.f, 600.f, platformWidth * 2, platformHeight * 2 },
            LIGHTGRAY
        },
        {
            { 400.f, 500.f, platformWidth * 2, platformHeight * 2 },
            LIGHTGRAY
        },
        {
            { 600.f, 400.f, platformWidth * 2, platformHeight * 2 },
            LIGHTGRAY
        },
        {
            { 800.f, 300.f, platformWidth * 2, platformHeight * 2 },
            LIGHTGRAY
        },
        {
            { 1000.f, 200.f, platformWidth * 2, platformHeight * 2 },
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
    player.origin = { player.width / 2, player.height };
    player.currentImage = 0;
    player.timeInIdleState = 0;
    player.timeInWalkingState = 0;

    while (!WindowShouldClose()) {
        // Update
        updateStates(player);
        updatePlayerPosition(player, items);
        updatePlayerImage(player, srcPlayer, playerWidth);
        srcPlayer.x = monkeys.at(player.currentImage).x;
        srcPlayer.y = monkeys.at(player.currentImage).y;

        // Render
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
                    DrawTexturePro(spriteSheet, srcPlatform, item.rect, originPlatform, 0, WHITE);
                }
            }

            // Player
            Rectangle destRect = {
                player.position.x,
                player.position.y,
                player.width,
                player.height
            };
            DrawTexturePro(spriteSheet, srcPlayer, destRect, player.origin, 0, WHITE);

            DrawText("Arrow keys for moving\nSpace key to jump\n'r' to restart", 30, 30, 44, WHITE);
        EndDrawing();
    }

    UnloadTexture(spriteSheet);
    CloseWindow();

    return 0;
}
