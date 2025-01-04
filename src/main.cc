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
} Scene;
Scene scene;

typedef struct
{
    Texture2D spriteSheet;
    Rectangle srcPlatform;
    Rectangle srcPlayer;
    Rectangle srcCollectible;
} SpriteCollection;
SpriteCollection spriteCollection;

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

void Init()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game 01");
    SetTargetFPS(60);

    spriteCollection.spriteSheet = LoadTexture("resources/monkeylad_further.png");

    spriteCollection.srcPlayer = {448, 208, 16, 24};
    short scalePlayer = 3;
    Player player;
    player.position = (Vector2){75.f, 600.f};
    player.width = spriteCollection.srcPlayer.width * scalePlayer;
    player.height = spriteCollection.srcPlayer.height * scalePlayer;
    player.speed = 0;
    player.canJump = false;
    player.state = State::Idle;
    player.origin = {player.width / 2, player.height};
    player.currentImage = 0;
    player.timeInIdleState = 0;
    player.timeInWalkingState = 0;
    player.score = 0;
    scene.player = player;

    spriteCollection.srcPlatform = {448, 33, 47, 8};
    short int scalePlatform = 2;
    scene.platforms = {
        {{SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50.f}},
        {{200.f, 600.f, spriteCollection.srcPlatform.width * scalePlatform,
          spriteCollection.srcPlatform.height * scalePlatform}},
        {{400.f, 500.f, spriteCollection.srcPlatform.width * scalePlatform,
          spriteCollection.srcPlatform.height * scalePlatform}},
        {{600.f, 400.f, spriteCollection.srcPlatform.width * scalePlatform,
          spriteCollection.srcPlatform.height * scalePlatform}},
        {{800.f, 300.f, spriteCollection.srcPlatform.width * scalePlatform,
          spriteCollection.srcPlatform.height * scalePlatform}},
        {{1000.f, 200.f, spriteCollection.srcPlatform.width * scalePlatform,
          spriteCollection.srcPlatform.height * scalePlatform}},
    };

    spriteCollection.srcCollectible = {592, 352, 16, 16};
    short int scaleCollectible = 2;
    scene.collectibles = {{
                              {440, 650, spriteCollection.srcCollectible.width * scaleCollectible,
                               spriteCollection.srcCollectible.height * scaleCollectible},
                              {16 / 2, 16 / 2},
                              false,
                          },
                          {
                              {540, 650, spriteCollection.srcCollectible.width * scaleCollectible,
                               spriteCollection.srcCollectible.height * scaleCollectible},
                              {16 / 2, 16 / 2},
                              false,
                          },
                          {
                              {640, 650, spriteCollection.srcCollectible.width * scaleCollectible,
                               spriteCollection.srcCollectible.height * scaleCollectible},
                              {16 / 2, 16 / 2},
                              false,
                          },
                          {
                              {740, 650, spriteCollection.srcCollectible.width * scaleCollectible,
                               spriteCollection.srcCollectible.height * scaleCollectible},
                              {16 / 2, 16 / 2},
                              false,
                          },
                          {
                              {840, 650, spriteCollection.srcCollectible.width * scaleCollectible,
                               spriteCollection.srcCollectible.height * scaleCollectible},
                              {16 / 2, 16 / 2},
                              false,
                          }};
}

void Update()
{
    float playerWidth = 16;
    std::vector<Vector2> monkeys = {
        {448, 208}, {464, 208}, {480, 208}, {496, 208}, {512, 208}, {528, 208},
    };
    updateUserState(scene);
    updatePlayerPosition(scene);
    updatePlayerImage(scene.player, spriteCollection.srcPlayer, playerWidth);
    spriteCollection.srcPlayer.x = monkeys.at(scene.player.currentImage).x;
    spriteCollection.srcPlayer.y = monkeys.at(scene.player.currentImage).y;

    for (auto &c : scene.collectibles) {
        Rectangle colD = {c.rec.x - c.origin.x, c.rec.y - c.origin.y, c.rec.width, c.rec.height};
        Rectangle playerD = {scene.player.position.x - scene.player.origin.x,
                             scene.player.position.y - scene.player.origin.y, scene.player.width, scene.player.height};

        if (CheckCollisionRecs(colD, playerD) && !c.isCollected) {
            c.isCollected = true;
            scene.player.score += 1;
        }
    }
}

void Draw()
{
    BeginDrawing();

    ClearBackground(SKYBLUE);

    // Ground
    auto g = scene.platforms.at(0);
    Rectangle groundDraw = {g.rec.x - (g.rec.width / 2), g.rec.y - g.rec.height, g.rec.width, g.rec.height};
    DrawRectangleRec(groundDraw, LIME);

    // Platforms
    auto subset = scene.platforms | std::views::drop(1) | std::views::take(scene.platforms.size() - 1);
    for (auto p : subset) {
        DrawTexturePro(spriteCollection.spriteSheet, spriteCollection.srcPlatform, p.rec,
                       {p.rec.width / 2, p.rec.height}, 0, WHITE);
    }

    // Player
    Rectangle playerDraw = {scene.player.position.x, scene.player.position.y, scene.player.width, scene.player.height};
    DrawTexturePro(spriteCollection.spriteSheet, spriteCollection.srcPlayer, playerDraw, scene.player.origin, 0, WHITE);

    // Collectibles
    for (auto c : scene.collectibles) {
        if (!c.isCollected) {
            DrawTexturePro(spriteCollection.spriteSheet, spriteCollection.srcCollectible, c.rec, c.origin, 0, WHITE);
        }
    }

    // UI
    DrawText("Arrow keys for moving\nSpace key to jump\n'r' to restart", 30, 30, 44, WHITE);
    DrawText(std::format("Score: {}", scene.player.score).c_str(), SCREEN_WIDTH - 300, 30, 44, WHITE);

    EndDrawing();
}

void Close()
{
    UnloadTexture(spriteCollection.spriteSheet);
    CloseWindow();
}

int main()
{
    Init();

    while (!WindowShouldClose()) {
        Update();
        Draw();
    }

    Close();

    return 0;
}
