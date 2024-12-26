#include <iostream>
#include <format>

#include "raylib.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

enum class Direction {
    Up,
    Down,
    Left,
    Right,
    Idle
};

std::string direction_to_string(Direction dir)
{
    switch (dir) {
        case Direction::Left: return "Left";
        case Direction::Right: return "Right";
        case Direction::Up: return "Up";
        case Direction::Down: return "Down";
        case Direction::Idle: return "Idle";
        default: return "Unknown";
    }
}

typedef struct {
    Direction horizontal_direction;
    Direction vertical_direction;
    float x;
    float y;
    float width;
    float height; 
} Character;

int step = 0;

void update_character_position(Character& c, Rectangle& platform)
{
    auto f_screen_width = (float) GetScreenWidth();
    auto f_screen_height = (float) GetScreenHeight();

    const float min_x = c.width / 2;
    const float max_x = f_screen_width - (c.width / 2);

    // TODO: replace this with proper collision for the ground
    const float max_y = (f_screen_height - 50.f) - (c.height / 2);

    if (c.horizontal_direction == Direction::Left) {
        if (c.x < 0) {
            c.x = 0.f;
            c.horizontal_direction = Direction::Idle;
        }
        else if (
            (c.x + c.width) <= platform.x &&
            (c.y <= platform.y) &&
            (c.vertical_direction == Direction::Idle)) {
                c.vertical_direction = Direction::Down;
        }
        else {
            c.x -= 10.f;
        }
    }

    if (c.horizontal_direction == Direction::Right) {
        if (c.x > max_x) {
            c.x = max_x;
            c.horizontal_direction = Direction::Idle;
        }
        else if (
            (platform.x + platform.width) <= c.x &&
            (c.y <= platform.y) &&
            (c.vertical_direction == Direction::Idle)) {
                c.vertical_direction = Direction::Down;
        }
        else {
            c.x += 10.f;
        }
    }

    if (c.vertical_direction == Direction::Up) {
        c.y -= 15;
        if (c.y < 0) {
            c.y = 0.f;
        }
        ++step;
    }

    if (c.vertical_direction == Direction::Down) {
        if (c.y > max_y) {
            c.y = max_y;
            c.vertical_direction = Direction::Idle;
        }
        else if (
            (platform.x - (platform.width / 2) - (c.width / 2)) <= c.x && c.x <= (platform.x + (platform.width / 2) + (c.width / 2)) &&
            (platform.y - (platform.height / 2) - (c.height / 2)) <= c.y && c.y <= (platform.y + (platform.height / 2))) {
                c.vertical_direction = Direction::Idle;
        }
        else {
            c.y += 6;
        }
    }
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game 01");
    SetTargetFPS(60);

    float velocity = 20.f;

    Rectangle platform = {
        GetScreenWidth() * 0.3f,
        GetScreenHeight() * 0.75f,
        100.f,
        20.f
    };

    float rect_width = 50.f;
    float rect_height = 50.f;
    Character character;
    character.horizontal_direction = Direction::Idle;
    character.vertical_direction = Direction::Down;
    character.x = (GetScreenWidth() / 2);
    character.y = (GetScreenHeight() / 2);
    character.width = rect_width;
    character.height = rect_height;

    while (!WindowShouldClose()) {
#pragma region PROCESS INPUT
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            character.horizontal_direction = Direction::Left;
        }
        else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            character.horizontal_direction = Direction::Right;
        }
        else {
            character.horizontal_direction = Direction::Idle;
        }

        if (IsKeyPressed(KEY_SPACE) && character.vertical_direction == Direction::Idle) {
            character.vertical_direction = Direction::Up;
        }

        if (step > 15) {
            step = 0;
            character.vertical_direction = Direction::Down;
        }
#pragma endregion

        update_character_position(character, platform);
        
BeginDrawing();
        ClearBackground(BLACK);

        // Draw ground
        DrawRectangle(0, GetScreenHeight() - 50, GetScreenWidth(), 50, GREEN);

        // Draw platform
        DrawRectangle(platform.x - (platform.width / 2), platform.y - (character.height / 2), platform.width, platform.height, BLUE);

        // Draw character
        DrawRectangle(character.x - (character.width / 2), character.y - (character.height / 2), character.width, character.height, RED);
        // DrawRectangle(character.x - 10, character.y - 30, character.width / 5, character.height + 10, BLUE);
        // DrawRectangle(character.x + character.width, character.y - 30, character.width / 5, character.height + 10, BLUE);
        // DrawRectangle(character.x, character.y, character.width, 10.f, GOLD);

        DrawText(std::format("Horizontal : {}", direction_to_string(character.horizontal_direction)).c_str(), 10, 10, 20, LIGHTGRAY);
        DrawText(std::format("Vertical : {}", direction_to_string(character.vertical_direction)).c_str(), 10, 30, 20, LIGHTGRAY);
EndDrawing();
    }

    CloseWindow();

    return 0;
}
