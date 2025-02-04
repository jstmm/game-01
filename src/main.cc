#include <iostream>
#include <memory>
#include <ranges>
#include <vector>
#include <sstream>

#include "common.hh"

// ================================================================================================

void saveLevelFile(std::vector<Collectible> &collectibles)
{
    std::string output;
    for (auto c : collectibles) {
        output += std::to_string(static_cast<int>(c.rec.x));
        output += ", ";
        output += std::to_string(static_cast<int>(c.rec.y));
        output += '\n';
    }

    std::vector<char> cstr(output.begin(), output.end());
    cstr.push_back('\0');

    SaveFileText("level.csv", cstr.data());
}

void loadLevelFile(std::vector<Collectible> &collectibles)
{
    char *fileToString = LoadFileText("level.csv");

    if (!fileToString) {
        std::vector<Collectible> defaultCollectibles;
        defaultCollectibles.push_back(Collectible { (float) 440, (float) 670 });
        defaultCollectibles.push_back(Collectible { (float) 540, (float) 670 });
        defaultCollectibles.push_back(Collectible { (float) 640, (float) 670 });
        defaultCollectibles.push_back(Collectible { (float) 740, (float) 670 });
        defaultCollectibles.push_back(Collectible { (float) 840, (float) 670 });

        saveLevelFile(defaultCollectibles);

        fileToString = LoadFileText("level.csv");
    }

    std::istringstream stream(fileToString);
    int coordinate_x, coordinate_y;
    char comma;
    std::string buffer;
    while (std::getline(stream, buffer)) {
        std::istringstream line(buffer);
        line >> coordinate_x >> comma >> coordinate_y;
        collectibles.push_back(Collectible { (float) coordinate_x, (float) coordinate_y });
    }
    UnloadFileText(fileToString);
}

void Init()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game 01");
    SetTargetFPS(60);

    game.currentMode = Mode::Play;

    spriteCollection.spriteSheet = LoadTexture("assets/monkeylad_further.png");
    spriteCollection.iconsSpriteSheet = LoadTexture("assets/icons.png");

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

    loadLevelFile(scene.collectibles);

    spriteCollection.srcCollectible = {592, 352, 16, 16};
    short int scaleCollectible = 2;

    for (auto &c : scene.collectibles) {
        c.rec.width = spriteCollection.srcCollectible.width * scaleCollectible;
        c.rec.height = spriteCollection.srcCollectible.height * scaleCollectible;
        c.origin = {c.rec.width / 2, c.rec.height};
        c.isCollected = false;
    }
    scene.selectedCollectible = nullptr;

    Button collectibleButton;
    collectibleButton.texture = &spriteCollection.spriteSheet;
    collectibleButton.source = {592, 352, 16, 16};
    collectibleButton.origin = { 25, 50 };
    collectibleButton.destination = { SCREEN_WIDTH / 2, 60, 60, 60 };
    collectibleButton.isSelected = false;
    game.editor.buttons.push_back(collectibleButton);

    Button saveButton;
    saveButton.texture = &spriteCollection.iconsSpriteSheet;
    saveButton.source = { 1194, 1095, 60, 65 };
    saveButton.origin = { 25, 50 };
    saveButton.destination = { (SCREEN_WIDTH / 2) + 60, 60, 60, 60 };
    saveButton.isSelected = false;
    game.editor.buttons.push_back(saveButton);

    game.editor.buttonSelected = -1;
}

// ================================================================================================

void updateUserState()
{
    if (game.currentMode == Mode::Play)
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

        if (IsKeyDown(KEY_ONE)) {
            scene.collectibles = {{{440, SCREEN_HEIGHT - 50.f}},
                            {{540, SCREEN_HEIGHT - 50.f}},
                            {{640, SCREEN_HEIGHT - 50.f}},
                            {{740, SCREEN_HEIGHT - 50.f}},
                            {{840, SCREEN_HEIGHT - 50.f}}};
        }

        if (IsKeyDown(KEY_TWO)) {
            scene.collectibles = {{{440, SCREEN_HEIGHT - 70.f}},
                            {{540, SCREEN_HEIGHT - 50.f}},
                            {{640, SCREEN_HEIGHT - 70.f}},
                            {{740, SCREEN_HEIGHT - 50.f}},
                            {{840, SCREEN_HEIGHT - 70.f}}};
        }

        if (IsKeyDown(KEY_ONE) || IsKeyDown(KEY_TWO)) {
            for (auto &c : scene.collectibles) {
                short int scaleCollectible = 2;
                c.rec.width = spriteCollection.srcCollectible.width * scaleCollectible;
                c.rec.height = spriteCollection.srcCollectible.height * scaleCollectible;
                c.origin = {c.rec.width / 2, c.rec.height};
                c.isCollected = false;
            }
        }

        if (IsKeyPressed(KEY_E)) {
            game.currentMode = Mode::Edit;
        }
    } else if (game.currentMode == Mode::Edit) {
        if (IsKeyPressed(KEY_E)) {
            game.currentMode = Mode::Play;
        }
    }
}

void updatePlayerPosition()
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

void Update()
{
    float playerWidth = 16;
    std::vector<Vector2> monkeys = {
        {448, 208}, {464, 208}, {480, 208}, {496, 208}, {512, 208}, {528, 208},
    };
    updateUserState();
    updatePlayerPosition();
    updatePlayerImage(scene.player, spriteCollection.srcPlayer, playerWidth);
    spriteCollection.srcPlayer.x = monkeys.at(scene.player.currentImage).x;
    spriteCollection.srcPlayer.y = monkeys.at(scene.player.currentImage).y;

    for (auto &c : scene.collectibles) {
        Rectangle colD = {c.rec.x - c.origin.x, c.rec.y - c.origin.y, c.rec.width, c.rec.height};
        Rectangle playerD = {scene.player.position.x - scene.player.origin.x,
                             scene.player.position.y - scene.player.origin.y, scene.player.width, scene.player.height};

        if (game.currentMode == Mode::Play) {
            if (CheckCollisionRecs(colD, playerD) && !c.isCollected) {
                c.isCollected = true;
                scene.player.score += 1;
            }
        }

        if (game.currentMode == Mode::Edit) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && game.editor.buttonSelected == -1) {
                if (CheckCollisionPointRec({(float)GetMouseX(), (float)GetMouseY()}, colD) &&
                    scene.selectedCollectible == nullptr) {
                    scene.selectedCollectible = &c;
                }
            }
            else {
                if (scene.selectedCollectible != nullptr) {
                    scene.selectedCollectible = nullptr;
                }
            }
        }
    }

    if (scene.selectedCollectible) {
        (*scene.selectedCollectible).rec.x = (float)GetMouseX();
        (*scene.selectedCollectible).rec.y = (float)GetMouseY();
    }

    bool buttonStateChange = false;

    if (game.currentMode == Mode::Edit) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int i = 0;
            for (auto &b : game.editor.buttons) {
                if (CheckCollisionPointRec({(float)GetMouseX(), (float)GetMouseY()}, { b.destination.x - b.origin.x, b.destination.y - b.origin.y, b.destination.width, b.destination.height })) {
                    if (b.isSelected) {
                        b.isSelected = false;
                        game.editor.buttonSelected = -1;
                    }
                    else {
                        b.isSelected = true;
                        game.editor.buttonSelected = i;
                    }
                    buttonStateChange = true;
                    break;
                }
                ++i;
            }

            if (!buttonStateChange && game.editor.buttonSelected == 0) {
                Collectible c;
                c.rec = { (float)GetMouseX(), (float)GetMouseY() };
                c.rec.width = spriteCollection.srcCollectible.width * 2;
                c.rec.height = spriteCollection.srcCollectible.height * 2;
                c.origin = {c.rec.width / 2, c.rec.height};
                c.isCollected = false;
                scene.collectibles.push_back(c);
            }

            if (game.editor.buttonSelected == 1) {
                saveLevelFile(scene.collectibles);

                Button &b = game.editor.buttons.at(game.editor.buttonSelected);
                b.isSelected = false;
                game.editor.buttonSelected = -1;
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            if (game.editor.buttonSelected != -1) {
                Button &b = game.editor.buttons.at(game.editor.buttonSelected);
                b.isSelected = false;
                game.editor.buttonSelected = -1;
            }
        }
    }
}

// ================================================================================================

void Draw()
{
    BeginDrawing();

    // Environment
    ClearBackground(SKYBLUE);

    auto g = scene.platforms.at(0);
    Rectangle groundDraw = {g.rec.x - (g.rec.width / 2), g.rec.y - g.rec.height, g.rec.width, g.rec.height};
    DrawRectangleRec(groundDraw, LIME);

    auto subset = scene.platforms | std::views::drop(1) | std::views::take(scene.platforms.size() - 1);
    for (auto p : subset) {
        DrawTexturePro(spriteCollection.spriteSheet, spriteCollection.srcPlatform, p.rec,
                       {p.rec.width / 2, p.rec.height}, 0, WHITE);
    }

    for (auto c : scene.collectibles) {
        if (!c.isCollected) {
            DrawTexturePro(spriteCollection.spriteSheet, spriteCollection.srcCollectible, c.rec, c.origin, 0, WHITE);
            if (game.currentMode == Mode::Edit) {
                DrawRectangleLinesEx({ c.rec.x - c.origin.x, c.rec.y - c.origin.y, c.rec.width, c.rec.height }, 1.f, BLACK);
            }
        }
    }

    // Player
    Rectangle playerDraw = {scene.player.position.x, scene.player.position.y, scene.player.width, scene.player.height};
    DrawTexturePro(spriteCollection.spriteSheet, spriteCollection.srcPlayer, playerDraw, scene.player.origin, 0, WHITE);

    // UI
    if (game.currentMode == Mode::Play) {
        DrawText("Arrow keys for moving\nSpace key to jump\n'r' to restart\n'e' to enter edit mode", 30, 30, 30, YELLOW);
        DrawText(std::format("Score: {}", scene.player.score).c_str(), SCREEN_WIDTH - 200, 30, 30, YELLOW);
    }

    if (game.currentMode == Mode::Edit) {
        DrawText("Edit Mode\n(press 'e' to exit)", 30, 30, 30, BLACK);

        for (auto &button : game.editor.buttons) {
            DrawRectanglePro(button.destination, button.origin, 0, (button.isSelected ? RED : PINK));
            DrawTexturePro(*(button.texture), button.source, button.destination, button.origin, 0, WHITE);
        }
    }

    EndDrawing();
}

// ================================================================================================

void Close()
{
    UnloadTexture(spriteCollection.spriteSheet);
    UnloadTexture(spriteCollection.iconsSpriteSheet);
    CloseWindow();
}

// ================================================================================================

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
