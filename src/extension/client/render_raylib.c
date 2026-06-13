#include "client/input.h"
#include "raylib.h"
#include "shared/types.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "render.h"

#define CELL_WIDTH 64
#define FONT_SIZE 40

#define SCREEN_WIDTH ((2 * BOARD_SIZE + 3) * CELL_WIDTH)
#define SCREEN_HEIGHT ((BOARD_SIZE + 6) * CELL_WIDTH)
#define FRAME_RATE 120

typedef struct {
    Texture2D textures[NUM_SHIPS];
} GameAssets;

typedef struct {
    bool is_dragging[NUM_SHIPS];
    bool is_rotated[NUM_SHIPS];
    Rectangle ship_rectangles[NUM_SHIPS];
} UiState;

static GameAssets assets;
static UiState ui_state;

typedef struct {
    int x;
    int y;
} ScreenCoord;

extern InputData input_state;

ScreenCoord cell_coordinates(int column, int row, bool is_board_1) {
    int left_offset = is_board_1 ? CELL_WIDTH : (BOARD_SIZE + 2) * CELL_WIDTH;
    return (ScreenCoord){
        .x = left_offset + CELL_WIDTH * column,
        .y = CELL_WIDTH + CELL_WIDTH * row,
    };
}

Rectangle cell_bounds(int column, int row, bool is_board_1) {
    ScreenCoord coords = cell_coordinates(column, row, is_board_1);
    Rectangle bounds = {
        .x = (float)coords.x,
        .y = (float)coords.y,
        .width = (float)CELL_WIDTH,
        .height = (float)CELL_WIDTH,
    };
    return bounds;
}

ScreenCoord ship_coordinates(int column, int row) {
    return (ScreenCoord){
        .x = CELL_WIDTH + CELL_WIDTH * column,
        .y = CELL_WIDTH + CELL_WIDTH * row,
    };
}

Rectangle ship_bounds(int column, int row, int width, int height,
                      bool is_board_1) {
    ScreenCoord coords = cell_coordinates(column, row, is_board_1);
    Rectangle bounds = {
        .x = (float)coords.x,
        .y = (float)coords.y,
        .width = (float)width,
        .height = (float)height,
    };
    return bounds;
}

bool init_graphics(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Battleship");

    SetTargetFPS(FRAME_RATE);

    char image_paths[NUM_SHIPS][50] = {
        "./assets/ShipCarrierHull.png",   "./assets/ShipBattleshipHull.png",
        "./assets/ShipCruiserHull.png",   "./assets/ShipSubMarineHull.png",
        "./assets/ShipDestroyerHull.png",
    };

    float scale_factors[NUM_SHIPS] = {
        1.3f,
        1.5f,
        1.5f,
        1.5f,
        1.3f,
    };

    for (int i = 0; i < NUM_SHIPS; i++) {
        Image image = LoadImage(image_paths[i]);
        ImageResizeNN(&image, image.width * scale_factors[i], image.height * scale_factors[i]);
        assets.textures[i] = LoadTextureFromImage(image);
        UnloadImage(image);

        if (!IsTextureValid(assets.textures[i])) {
            // Unload all the textures that have already been loaded.
            for (int j = 0; j < i; j++) {
                UnloadTexture(assets.textures[j]);
            }
            TraceLog(LOG_ERROR, "Failed to load texture!");
            return false;
        }
    }

    int y = 750;
    for (int i = 0; i < NUM_SHIPS; i++) {
        ui_state.is_dragging[i] = false;
        ui_state.ship_rectangles[i] = (Rectangle) {
            .x = (i + 1) * 100,
            .y = y,
            .width = assets.textures[i].width,
            .height = assets.textures[i].height,
        };
    }

    return true;
}

bool is_window_open(void) { return !WindowShouldClose(); }

void render_frame(const ClientState *state) {
    for (int i = 0; i < NUM_SHIPS; i++) {
        Rectangle *rectangle = &ui_state.ship_rectangles[i];
        // Camera2D ship_camera = {0};
        // ship_camera.zoom = 1.0f;
        // ship_camera.rotation = 270.0f;
        // Vector2 ship_pos = (Vector2) {
        //     .x = rectangle->x,
        //     .y = rectangle->y,
        // };
        // ship_camera.target = ship_pos;
        // ship_camera.offset = ship_pos;

        // Vector2 worldMouse = GetScreenToWorld2D(GetMousePosition(), ship_camera);
        Vector2 mouse = GetMousePosition();

        bool is_hovering = CheckCollisionPointRec(mouse, *rectangle);
        if (is_hovering && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            ui_state.is_dragging[i] = true;
            TraceLog(LOG_INFO, "started dragging");
        }
        if (is_hovering && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            ui_state.is_rotated[i] = !ui_state.is_rotated[i];
        }

        if (ui_state.is_dragging[i]) {
            Vector2 mouse_delta = GetMouseDelta();
            rectangle->x += mouse_delta.x;
            rectangle->y += mouse_delta.y;

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                ui_state.is_dragging[i] = false;
                TraceLog(LOG_INFO, "stopped dragging");
            }
        }
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText("Your board", CELL_WIDTH, 5, FONT_SIZE, DARKGRAY);
    DrawText("Opponent's board", CELL_WIDTH * 12, 5, FONT_SIZE, DARKGRAY);

    GuiSetState(STATE_DISABLED);
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Rectangle bounds = cell_bounds(x, y, true);
            if (GuiButton(bounds, "")) {
                char f[16];
                sprintf(f, "x: %d, y: %d", x, y);
                TraceLog(LOG_INFO, f);
            }
        }
    }
    GuiSetState(STATE_NORMAL);

    // GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE);

    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            int original_normal = GuiGetStyle(BUTTON, BASE_COLOR_NORMAL);
            int original_focused = GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED);
            int original_text_normal = GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL);
            int original_text_focused = GuiGetStyle(BUTTON, TEXT_COLOR_FOCUSED);

            switch (get_cell(state->target_board, x, y)) {
            case CELL_MISS:
                GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(WHITE));
                GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(WHITE));
                GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
                break;
            case CELL_HIT:
                GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(RED));
                GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(RED));
                GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
                break;
            default:
                break;
            }

            Rectangle bounds = cell_bounds(x, y, false);
            if (GuiButton(bounds, "")) {
                input_state = (InputData){
                    .grid_pos = {
                        .x = x,
                        .y = y
                    },
                    .type = INPUT_FIRE,
                };
                switch (get_cell(state->target_board, x, y)) {
                case CELL_WATER: // MISS
                    // TODO: INPUT
                    TraceLog(LOG_INFO, "Miss.");
                    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(WHITE));
                    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(WHITE));
                    break;
                case CELL_SHIP: // HIT
                    // TODO: INPUT
                    TraceLog(LOG_INFO, "HIT!!");
                    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(RED));
                    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(RED));
                    break;
                case CELL_HIT:
                case CELL_MISS: // ALREADY ATTEMPTED
                    TraceLog(LOG_INFO, "Already sent a missile there.");
                    break;
                }
            }
            // Resets global styles.
            GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, original_normal);
            GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, original_focused);
            GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, original_text_normal);
            GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, original_text_focused);
        }
    }

    // ScreenCoord coords = cell_coordinates(4, 3, false);
    // DrawTexture(carrier_texture, coords.x, coords.y, WHITE);

    // BeginMode2D(ship_camera);
    for (int i = 0; i < NUM_SHIPS; i++) {

        // DrawTexturePro(carrier_texture, carrier_box, carrier_rectangle,
        //                (Vector2){0.0f, 0.0f}, 270.0f, WHITE);
        // DrawTexture(assets.textures[i], ui_state.ship_rectangles[i].x,
        //             ui_state.ship_rectangles[i].y, WHITE);
        float angle = ui_state.is_rotated[i] ? 270.0f : 0.0f;
        Rectangle rectangle = ui_state.ship_rectangles[i];
        Rectangle ship_box = {
            .x = 0,
            .y = 0,
            .height = rectangle.height,
            .width = rectangle.width,
        };
        Vector2 origin = {
            .x = rectangle.width / 2,
            .y = rectangle.height / 2,
        };
        DrawTexturePro(assets.textures[i], ship_box, rectangle, origin, angle, WHITE);
    }
    // EndMode2D();

    EndDrawing();
}

/* Frees textures, closes windows, resets terminal. */
void cleanup_graphics(void) {
    for (int i = 0; i < NUM_SHIPS; i++) {
        UnloadTexture(assets.textures[i]);
    }
    CloseWindow();
}
