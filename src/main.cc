#include <iostream>
#include <ranges>
#include <vector>

#include "raylib.h"

#define GRAVITY 600
#define PLAYER_JUMP_SPD 400.0f
#define PLAYER_HOR_SPD 400.0f
#define SCREEN_WIDTH 1280.0f
#define SCREEN_HEIGHT 720.0f

enum class State
{
    Idle,
    MovingLeft,
    MovingRight
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
} Player;

typedef struct
{
    Rectangle rec;
    Color colour;
} Platform;

typedef struct
{
    Rectangle rec;
    Vector2 origin;
    bool isCollected;
} Collectible;

typedef struct
{
    Player &player;
    std::vector<Platform> &platforms;
    std::vector<Collectible> &collectibles;
} Scene;

bool areColoursEqual(Color c1, Color c2)
{
    return ((c1.r == c2.r) && (c1.g == c2.g) && (c1.b == c2.b) && (c1.a == c2.a));
}

void updateUserState(Scene &scene)
{
    if (IsKeyPressed(KEY_R)) {
        scene.player.position.x = 75.f;
        scene.player.position.y = 600.f;
        scene.player.state = State::Idle;
        scene.player.speed = 0;
        scene.player.canJump = false;
        scene.player.currentImage = 0;
        scene.player.timeInIdleState = 0;
        scene.player.timeInWalkingState = 0;
        for (auto &c : scene.collectibles) {
            c.isCollected = false;
        }
        return;
    }

    if (IsKeyPressed(KEY_SPACE) && scene.player.canJump) {
        scene.player.speed = -PLAYER_JUMP_SPD;
        scene.player.canJump = false;
    }

    if (!scene.player.canJump) {
        scene.player.timeInIdleState = 0;
        scene.player.timeInWalkingState = 0;
    }

    if (IsKeyDown(KEY_LEFT)) {
        scene.player.state = State::MovingLeft;
        if (scene.player.canJump) {
            ++scene.player.timeInWalkingState;
            scene.player.timeInIdleState = 0;
        }
    }
    else if (IsKeyDown(KEY_RIGHT)) {
        scene.player.state = State::MovingRight;
        if (scene.player.canJump) {
            ++scene.player.timeInWalkingState;
            scene.player.timeInIdleState = 0;
        }
    }
    else {
        scene.player.state = State::Idle;
        if (scene.player.canJump) {
            ++scene.player.timeInIdleState;
            scene.player.timeInWalkingState = 0;
        }
    }
}

void updatePlayerPosition(Scene &scene)
{
    const float deltaTime = GetFrameTime();
    const float minX = scene.player.width / 2;
    const float maxX = SCREEN_WIDTH - (scene.player.width / 2);

    bool hitObstacle = false;

    if (scene.player.state == State::MovingLeft)
        scene.player.position.x -= PLAYER_HOR_SPD * deltaTime;

    if (scene.player.state == State::MovingRight) {
        scene.player.position.x += PLAYER_HOR_SPD * deltaTime;
    }

    for (Platform &i : scene.platforms) {
        if ((i.rec.x - (i.rec.width / 2) <= scene.player.position.x + (scene.player.width / 2)) &&
            (scene.player.position.x - (scene.player.width / 2) <= i.rec.x + (i.rec.width / 2)) &&
            (scene.player.position.y <= i.rec.y - i.rec.height) &&
            (scene.player.position.y + (scene.player.speed * deltaTime) >= i.rec.y - i.rec.height)) {
            hitObstacle = true;
            scene.player.speed = 0.0f;
            scene.player.position.y = i.rec.y - i.rec.height;
            break;
        }
    }

    if (!hitObstacle) {
        scene.player.position.y += scene.player.speed * deltaTime;
        scene.player.speed += GRAVITY * deltaTime;
        scene.player.canJump = false;
    }
    else {
        scene.player.canJump = true;
    }

    if (scene.player.position.x <= minX)
        scene.player.position.x = minX;
    if (scene.player.position.x >= maxX)
        scene.player.position.x = maxX;
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

void updatePlayerImage(Player &player, Rectangle &srcPlayer, int playerWidth)
{
    if (!player.canJump) {
        player.currentImage = 3;
        srcPlayer.width = (player.state == State::MovingLeft)    ? -playerWidth
                          : (player.state == State::MovingRight) ? playerWidth
                                                                 : srcPlayer.width;
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



    float playerWidth = 16;
    Rectangle srcPlayer = {448, 208, 16, 24};
    std::vector<Vector2> monkeys = {
        {448, 208}, {464, 208}, {480, 208}, {496, 208}, {512, 208}, {528, 208},
    };
    short scale = 3;
    Player player;
    player.position = (Vector2){75.f, 600.f};
    player.width = srcPlayer.width * scale;
    player.height = srcPlayer.height * scale;
    player.speed = 0;
    player.canJump = false;
    player.state = State::Idle;
    player.origin = {player.width / 2, player.height};
    player.currentImage = 0;
    player.timeInIdleState = 0;
    player.timeInWalkingState = 0;


    Rectangle srcCollectible = { 592, 352, 16, 16 };

    Rectangle srcPlatform = {448, 33, 47, 8};
    // Initialise entities
    std::vector<Platform> platforms = {
        {{SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50.f}, LIME},
        {{200.f, 600.f, srcPlatform.width * 2, srcPlatform.height * 2}, LIGHTGRAY},
        {{400.f, 500.f, srcPlatform.width * 2, srcPlatform.height * 2}, LIGHTGRAY},
        {{600.f, 400.f, srcPlatform.width * 2, srcPlatform.height * 2}, LIGHTGRAY},
        {{800.f, 300.f, srcPlatform.width * 2, srcPlatform.height * 2}, LIGHTGRAY},
        {{1000.f, 200.f, srcPlatform.width * 2, srcPlatform.height * 2}, LIGHTGRAY},
    };
    Vector2 originPlatform = {100.f / 2, 20.f};



    std::vector<Collectible> collectibles = {{{ 440, 650, 30, 40 }, { 16/2, 16/2 }, false, },
                                             {{ 540, 650, 30, 40 }, { 16/2, 16/2 }, false, },
                                             {{ 640, 650, 30, 40 }, { 16/2, 16/2 }, false, },
                                             {{ 740, 650, 30, 40 }, { 16/2, 16/2 }, false, },
                                             {{ 840, 650, 30, 40 }, { 16/2, 16/2 }, false, }};

    Scene scene = {player, platforms, collectibles};

    unsigned int score = 0;

    while (!WindowShouldClose()) {
        // Update
        updateUserState(scene);
        updatePlayerPosition(scene);
        updatePlayerImage(player, srcPlayer, playerWidth);
        srcPlayer.x = monkeys.at(player.currentImage).x;
        srcPlayer.y = monkeys.at(player.currentImage).y;

        for (auto &c : collectibles) {
            Rectangle colD = {
                c.rec.x - c.origin.x,
                c.rec.y - c.origin.y,
                c.rec.width,
                c.rec.height
            };
            Rectangle playerD = {
                player.position.x - player.origin.x,
                player.position.y - player.origin.y,
                player.width,
                player.height
            };

            if (CheckCollisionRecs(colD, playerD) && !c.isCollected) {
                c.isCollected = true;
                score += 1;
            }
        }

        // Render
        BeginDrawing();

        ClearBackground(SKYBLUE);

        // Ground
        auto g = platforms.at(0);
        Rectangle groundDraw = {g.rec.x - (g.rec.width / 2), g.rec.y - g.rec.height, g.rec.width, g.rec.height};
        DrawRectangleRec(groundDraw, g.colour);

        // Platforms
        auto subset = platforms | std::views::drop(1) | std::views::take(platforms.size() - 1);
        for (auto p : subset) {
            DrawTexturePro(spriteSheet, srcPlatform, p.rec, originPlatform, 0, WHITE);
        }

        // Player
        Rectangle playerDraw = {player.position.x, player.position.y, player.width, player.height};
        DrawTexturePro(spriteSheet, srcPlayer, playerDraw, player.origin, 0, WHITE);

        // Collectibles
        for (auto c : collectibles) {
            if (!c.isCollected) {
                DrawTexturePro(spriteSheet, srcCollectible, c.rec, c.origin, 0, WHITE);
            }
        }

        // UI
        DrawText("Arrow keys for moving\nSpace key to jump\n'r' to restart", 30, 30, 44, WHITE);
        DrawText(std::format("Score: {}", score).c_str(), SCREEN_WIDTH - 300, 30, 44, WHITE);

        EndDrawing();
    }

    UnloadTexture(spriteSheet);
    CloseWindow();

    return 0;
}
