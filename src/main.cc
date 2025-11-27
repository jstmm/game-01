#include <cstddef>
#include <iostream>
#include <format>
#include <string>
#include <ranges>
#include <vector>
#include <sstream>
#include <filesystem>

#include "common.h"

// ================================================================================================

static Game game;
static Scene scene;
static SpriteCollection sprite_collection;

void save_level_file(const std::vector<Collectible>& collectibles)
{
    std::string output;
    for (Collectible c : collectibles) {
        output += std::to_string(static_cast<int>(c.rec.x));
        output += ", ";
        output += std::to_string(static_cast<int>(c.rec.y));
        output += '\n';
    }

    std::vector<char> cstr(output.begin(), output.end());
    cstr.push_back('\0');

    SaveFileText("level.csv", cstr.data());
}

void load_level_file(std::vector<Collectible>& collectibles)
{
    char* file_to_string = LoadFileText("level.csv");

    if (!file_to_string) {
        std::vector<Collectible> default_collectibles {
            { 440.f, 670.f },
            { 540.f, 670.f },
            { 640.f, 670.f },
            { 740.f, 670.f },
            { 840.f, 670.f }
        };
        save_level_file(default_collectibles);
        file_to_string = LoadFileText("level.csv");
    }

    std::istringstream stream(file_to_string);
    int coordinate_x, coordinate_y;
    char comma;
    std::string buffer;
    while (std::getline(stream, buffer)) {
        std::istringstream line(buffer);
        line >> coordinate_x >> comma >> coordinate_y;
        collectibles.push_back({ static_cast<float>(coordinate_x), static_cast<float>(coordinate_y) });
    }
    UnloadFileText(file_to_string);
}

void init()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game 01");
    SetTargetFPS(60);

    game.current_mode = Mode::Play;

    sprite_collection.sprite_sheet = LoadTexture("assets/monkeylad_further.png");
    sprite_collection.icons_sprite_sheet = LoadTexture("assets/icons.png");

    sprite_collection.src_player = {448, 208, 16, 24};
    float scale_player = 3.f;
    float player_width = sprite_collection.src_player.width * scale_player;
    float player_height = sprite_collection.src_player.height * scale_player;
    scene.player = Player {
        .rec = {
            .x = 75.f,
            .y = 600.f,
            .width = player_width,
            .height = player_height
        },
        .origin = {player_width / 2, player_height},
        .state = State::Idle,
        .speed = 0,
        .can_jump = false,
        .current_image = 0,
        .time_in_idle_state = 0,
        .time_in_walking_state = 0,
        .score = 0,
    };

    sprite_collection.src_platform = {448, 33, 47, 8};
    float scale_platform = 2.f;
    scene.platforms = {
        {{SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50.f}},
        {{200.f,
          600.f,
          sprite_collection.src_platform.width * scale_platform,
          sprite_collection.src_platform.height * scale_platform}},
        {{400.f,
          500.f,
          sprite_collection.src_platform.width * scale_platform,
          sprite_collection.src_platform.height * scale_platform}},
        {{600.f,
          400.f,
          sprite_collection.src_platform.width * scale_platform,
          sprite_collection.src_platform.height * scale_platform}},
        {{800.f,
          300.f,
          sprite_collection.src_platform.width * scale_platform,
          sprite_collection.src_platform.height * scale_platform}},
        {{1000.f,
          200.f,
          sprite_collection.src_platform.width * scale_platform,
          sprite_collection.src_platform.height * scale_platform}},
    };

    for (auto& platform : scene.platforms) {
        platform.origin = {
            .x = platform.rec.width / 2,
            .y = platform.rec.height
        };
    }

    load_level_file(scene.collectibles);

    sprite_collection.src_collectible = {592, 352, 16, 16};
    float scale_collectible = 2.f;

    for (auto& c : scene.collectibles) {
        c.rec.width = sprite_collection.src_collectible.width * scale_collectible;
        c.rec.height = sprite_collection.src_collectible.height * scale_collectible;
        c.origin = {c.rec.width / 2, c.rec.height};
        c.is_collected = false;
    }
    scene.selected_collectible = nullptr;

    Button collectible_button {
        .texture = &sprite_collection.sprite_sheet,
        .source = {592, 352, 16, 16},
        .origin = {25, 50},
        .destination = {SCREEN_WIDTH / 2, 60, 60, 60},
        .is_selected = false
    };
    game.editor.buttons.push_back(collectible_button);

    Button save_button {
        .texture = &sprite_collection.icons_sprite_sheet,
        .source = {1194, 1095, 60, 65},
        .origin = {25, 50},
        .destination = {(SCREEN_WIDTH / 2) + 60, 60, 60, 60},
        .is_selected = false
    };
    game.editor.buttons.push_back(save_button);

    game.editor.button_selected = -1;
}

// ================================================================================================

void update_user_state()
{
    if (WindowShouldClose() || IsKeyPressed(KEY_Q)) {
        game.is_running = false;
        return;
    }

    if (game.current_mode == Mode::Play) {
        if (IsKeyPressed(KEY_R)) {
            scene.player.rec.x = 75.f;
            scene.player.rec.y = 600.f;
            scene.player.state = State::Idle;
            scene.player.speed = 0;
            scene.player.can_jump = false;
            scene.player.current_image = 0;
            scene.player.time_in_idle_state = 0;
            scene.player.time_in_walking_state = 0;
            for (auto& c : scene.collectibles) {
                c.is_collected = false;
            }
            return;
        }

        if (IsKeyPressed(KEY_SPACE) && scene.player.can_jump) {
            scene.player.speed = -PLAYER_JUMP_SPD;
            scene.player.can_jump = false;
        }

        if (!scene.player.can_jump) {
            scene.player.time_in_idle_state = 0;
            scene.player.time_in_walking_state = 0;
        }

        if (IsKeyDown(KEY_LEFT)) {
            scene.player.state = State::MovingLeft;
            if (scene.player.can_jump) {
                ++scene.player.time_in_walking_state;
                scene.player.time_in_idle_state = 0;
            }
        } else if (IsKeyDown(KEY_RIGHT)) {
            scene.player.state = State::MovingRight;
            if (scene.player.can_jump) {
                ++scene.player.time_in_walking_state;
                scene.player.time_in_idle_state = 0;
            }
        } else {
            scene.player.state = State::Idle;
            if (scene.player.can_jump) {
                ++scene.player.time_in_idle_state;
                scene.player.time_in_walking_state = 0;
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
            for (auto& c : scene.collectibles) {
                float scale_collectible = 2.f;
                c.rec.width = sprite_collection.src_collectible.width * scale_collectible;
                c.rec.height = sprite_collection.src_collectible.height * scale_collectible;
                c.origin = {c.rec.width / 2, c.rec.height};
                c.is_collected = false;
            }
        }

        if (IsKeyPressed(KEY_E)) {
            game.current_mode = Mode::Edit;
        }

        if (IsKeyPressed(KEY_O)) {
            game.show_origins = !game.show_origins;
        }

        return;
    }

    if (game.current_mode == Mode::Edit) {
        if (IsKeyPressed(KEY_E)) {
            game.current_mode = Mode::Play;
        }
        return;
    }
}

void update_player_position()
{
    const float delta_time = GetFrameTime();
    const float minX = scene.player.rec.width / 2;
    const float maxX = SCREEN_WIDTH - (scene.player.rec.width / 2);

    bool hit_obstacle = false;

    if (scene.player.state == State::MovingLeft)
        scene.player.rec.x -= PLAYER_HOR_SPD * delta_time;

    if (scene.player.state == State::MovingRight) {
        scene.player.rec.x += PLAYER_HOR_SPD * delta_time;
    }

    for (Platform& i : scene.platforms) {
        if ((i.rec.x - (i.rec.width / 2) <= scene.player.rec.x + (scene.player.rec.width / 2))
            && (scene.player.rec.x - (scene.player.rec.width / 2) <= i.rec.x + (i.rec.width / 2))
            && (scene.player.rec.y <= i.rec.y - i.rec.height)
            && (scene.player.rec.y + (scene.player.speed * delta_time)
                >= i.rec.y - i.rec.height)) {
            hit_obstacle = true;
            scene.player.speed = 0.0f;
            scene.player.rec.y = i.rec.y - i.rec.height;
            break;
        }
    }

    if (!hit_obstacle) {
        scene.player.rec.y += scene.player.speed * delta_time;
        scene.player.speed += GRAVITY * delta_time;
        scene.player.can_jump = false;
    } else {
        scene.player.can_jump = true;
    }

    if (scene.player.rec.x <= minX)
        scene.player.rec.x = minX;
    if (scene.player.rec.x >= maxX)
        scene.player.rec.x = maxX;
}

int image_ref_for_time_in_walking_state(const int time)
{
    const std::vector<int> image_refs = {2, 3, 4, 5};
    constexpr int time_per_image = 6;
    const int duration = static_cast<int>(image_refs.size()) * time_per_image;
    int result = image_refs.at(0);
    for (size_t i = 0; i < image_refs.size(); ++i) {
        if ((time % duration) < (time_per_image * (i + 1))) {
            result = image_refs.at(i);
            break;
        }
    }
    return result;
}

void update_player_image(Player& player, Rectangle& src_player, int player_width)
{
    if (!player.can_jump) {
        player.current_image = 3;
        src_player.width = (player.state == State::MovingLeft)    ? -player_width
                          : (player.state == State::MovingRight) ? player_width
                                                                 : src_player.width;
        return;
    }

    if (player.state == State::MovingLeft) {
        player.current_image = image_ref_for_time_in_walking_state(player.time_in_walking_state);
        src_player.width = -player_width;
        return;
    }

    if (player.state == State::MovingRight) {
        player.current_image = image_ref_for_time_in_walking_state(player.time_in_walking_state);
        src_player.width = player_width;
        return;
    }

    if (player.state == State::Idle) {
        if (player.time_in_idle_state < 120) {
            player.current_image = 1;
        } else {
            player.current_image = 0;
        }
    }
}

Rectangle apply_transform(auto c)
{
    return Rectangle {
        .x =      c.rec.x - c.origin.x,
        .y =      c.rec.y - c.origin.y,
        .width =  c.rec.width,
        .height = c.rec.height
    };
}

void update()
{
    float player_width = 16;
    auto monkeys = std::vector<Vector2> {
        {448, 208},
        {464, 208},
        {480, 208},
        {496, 208},
        {512, 208},
        {528, 208},
    };
    update_user_state();
    update_player_position();
    update_player_image(scene.player, sprite_collection.src_player, player_width);
    sprite_collection.src_player.x = monkeys.at(scene.player.current_image).x;
    sprite_collection.src_player.y = monkeys.at(scene.player.current_image).y;

    for (Collectible& c : scene.collectibles) {
        Rectangle col_d = apply_transform(c);
        Rectangle player_d = apply_transform(scene.player);

        if (game.current_mode == Mode::Play) {
            if (CheckCollisionRecs(col_d, player_d) && !c.is_collected) {
                c.is_collected = true;
                scene.player.score += 1;
            }
        }

        if (game.current_mode == Mode::Edit) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && game.editor.button_selected == -1) {
                if (CheckCollisionPointRec({(float)GetMouseX(), (float)GetMouseY()}, col_d)
                    && scene.selected_collectible == nullptr) {
                    scene.selected_collectible = &c;
                }
            } else {
                if (scene.selected_collectible != nullptr) {
                    scene.selected_collectible = nullptr;
                }
            }
        }
    }

    if (scene.selected_collectible) {
        (*scene.selected_collectible).rec.x = (float)GetMouseX();
        (*scene.selected_collectible).rec.y = (float)GetMouseY();
    }

    bool button_state_change = false;

    if (game.current_mode == Mode::Edit) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int i = 0;
            for (auto& b : game.editor.buttons) {
                if (CheckCollisionPointRec({(float)GetMouseX(), (float)GetMouseY()},
                                           {b.destination.x - b.origin.x,
                                            b.destination.y - b.origin.y,
                                            b.destination.width,
                                            b.destination.height})) {
                    if (b.is_selected) {
                        b.is_selected = false;
                        game.editor.button_selected = -1;
                    } else {
                        b.is_selected = true;
                        game.editor.button_selected = i;
                    }
                    button_state_change = true;
                    break;
                }
                ++i;
            }

            if (!button_state_change && game.editor.button_selected == 0) {
                Collectible c;
                c.rec = {(float)GetMouseX(), (float)GetMouseY()};
                c.rec.width = sprite_collection.src_collectible.width * 2;
                c.rec.height = sprite_collection.src_collectible.height * 2;
                c.origin = {c.rec.width / 2, c.rec.height};
                c.is_collected = false;
                scene.collectibles.push_back(c);
            }

            if (game.editor.button_selected == 1) {
                save_level_file(scene.collectibles);

                Button& b = game.editor.buttons.at(game.editor.button_selected);
                b.is_selected = false;
                game.editor.button_selected = -1;
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            if (game.editor.button_selected != -1) {
                Button& b = game.editor.buttons.at(game.editor.button_selected);
                b.is_selected = false;
                game.editor.button_selected = -1;
            }
        }
    }
}

// ================================================================================================

void draw_origin(Rectangle rec)
{
    if (!game.show_origins) {
        return;
    };
    float width = 10, height = 10;
    DrawRectanglePro({
        .x = rec.x - (width/2),
        .y = rec.y - (height/2),
        .width = width,
        .height = height
    }, { 0, 0 }, 0, RED);
}

void draw()
{
    BeginDrawing();

    // Environment
    ClearBackground(SKYBLUE);

    auto ground = scene.platforms.at(0);
    DrawRectanglePro(ground.rec, ground.origin, 0, LIME);
    draw_origin(ground.rec);

    for (auto platform : scene.platforms | std::views::drop(1) | std::views::take(scene.platforms.size() - 1)) {
        DrawTexturePro(sprite_collection.sprite_sheet,
                       sprite_collection.src_platform,
                       platform.rec,
                       platform.origin,
                       0,
                       WHITE);
        draw_origin(platform.rec);
    }

    for (auto c : scene.collectibles) {
        if (!c.is_collected) {
            DrawTexturePro(sprite_collection.sprite_sheet,
                           sprite_collection.src_collectible,
                           c.rec,
                           c.origin,
                           0,
                           WHITE);
            if (game.current_mode == Mode::Edit) {
                DrawRectangleLinesEx(
                    {c.rec.x - c.origin.x, c.rec.y - c.origin.y, c.rec.width, c.rec.height},
                    1.f,
                    BLACK);
            }
            draw_origin(c.rec);
        }
    }

    // Player
    DrawTexturePro(sprite_collection.sprite_sheet,
                   sprite_collection.src_player,
                   scene.player.rec,
                   scene.player.origin,
                   0,
                   WHITE);
    draw_origin(scene.player.rec);

    // UI
    if (game.current_mode == Mode::Play) {
        DrawText("Arrow keys for moving\nSpace key to jump\n'r' to restart\n'e' to enter edit mode\n'o' to view origins",
                 30,
                 30,
                 30,
                 YELLOW);
        DrawText(std::format("Score: {}", scene.player.score).c_str(),
                 SCREEN_WIDTH - 200,
                 30,
                 30,
                 YELLOW);
    }

    if (game.current_mode == Mode::Edit) {
        DrawText("Edit Mode\n(press 'e' to exit)", 30, 30, 30, BLACK);

        for (auto& button : game.editor.buttons) {
            DrawRectanglePro(
                button.destination, button.origin, 0, (button.is_selected ? RED : PINK));
            DrawTexturePro(
                *(button.texture), button.source, button.destination, button.origin, 0, WHITE);
        }
    }

    EndDrawing();
}

// ================================================================================================

int main()
{
    init();

    while (game.is_running) {
        update();
        draw();
    }

    UnloadTexture(sprite_collection.sprite_sheet);
    UnloadTexture(sprite_collection.icons_sprite_sheet);
    CloseWindow();
}
