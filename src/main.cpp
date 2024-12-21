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

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game 01");
    SetTargetFPS(60);

    auto f_screen_width = (float) GetScreenWidth();
    auto f_screen_height = (float) GetScreenHeight();

    Color rect_color = RED;
    float velocity = 20.f;
    float rect_width = 50.f;
    float rect_height = 50.f;
    Rectangle character = {
        (GetScreenWidth() / 2) - (rect_width / 2),
        (GetScreenHeight() / 2) - (rect_height / 2),
        rect_width, rect_height
    };
    Rectangle platform = {
        GetScreenWidth() * 0.3f,
        GetScreenHeight() * 0.75f,
        100.f, 20.f
    };

    Direction vertical_direction = Direction::Down;
    Direction horizontal_direction = Direction::Idle;

    int step = 0;
    const float max_x = (f_screen_width - character.width);
    const float max_y = (f_screen_height - character.height) - 20;

    while (!WindowShouldClose()) {
#pragma region PROCESS INPUT
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            horizontal_direction = Direction::Left;
        }
        else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            horizontal_direction = Direction::Right;
        }
        else {
            horizontal_direction = Direction::Idle;
        }

        if (IsKeyPressed(KEY_SPACE) && vertical_direction == Direction::Idle) {
            vertical_direction = Direction::Up;
        }

        if (step > 15) {
            step = 0;
            vertical_direction = Direction::Down;
        }
#pragma endregion
#pragma region UPDATE POSITION
        if (character.y < 0) {
            character.y = 0.f;
        }

        if (horizontal_direction == Direction::Left) {
            if (character.x < 0) {
                character.x = 0.f;
                horizontal_direction = Direction::Idle;
            }
            else if ((character.x + character.width) <= platform.x && (character.y <= platform.y) && (vertical_direction == Direction::Idle)) {
                vertical_direction = Direction::Down;
            }
            else {
                character.x -= 10.f;
            }
        }

        if (horizontal_direction == Direction::Right) {
            if (character.x > max_x) {
                character.x = max_x;
                horizontal_direction = Direction::Idle;
            }
            else if ((platform.x + platform.width) <= character.x && (character.y <= platform.y) && (vertical_direction == Direction::Idle)) {
                vertical_direction = Direction::Down;
            }
            else {
                character.x += 10.f;
            }
        }

        if (vertical_direction == Direction::Up) {
            character.y -= 15;
            ++step;
        }

        if (vertical_direction == Direction::Down) {
            if (character.y > max_y) {
                character.y = max_y;
                vertical_direction = Direction::Idle;
            }
            else if ((platform.x - character.width) <= character.x && character.x <= (platform.x + platform.width) && character.y >= (platform.y - character.height)) {
                vertical_direction = Direction::Idle;
            }
            else {
                character.y += 6;
            }
        }
#pragma endregion
#pragma region DRAW
        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangle(0, GetScreenHeight() - 50, GetScreenWidth(), character.height, GREEN);

        DrawRectangle(platform.x, platform.y, platform.width, platform.height, BLUE);

        DrawRectangle(character.x, character.y, character.width, character.height, rect_color);
        DrawRectangle(character.x - 10, character.y - 30, character.width / 5, character.height + 10, BLUE);
        DrawRectangle(character.x + character.width, character.y - 30, character.width / 5, character.height + 10, BLUE);
        DrawRectangle(character.x, character.y, character.width, 10.f, GOLD);

        DrawText(std::format("Horizontal : {}", direction_to_string(horizontal_direction)).c_str(), 10, 10, 20, LIGHTGRAY);
        DrawText(std::format("Vertical : {}", direction_to_string(vertical_direction)).c_str(), 10, 30, 20, LIGHTGRAY);
        EndDrawing();
#pragma endregion
    }

    CloseWindow();

    return 0;
}
