#include "client/input.h"
#include "raylib.h"
#include "shared/board.h"
#include "shared/types.h"
#include <assert.h>
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
    Texture2D rotated_textures[NUM_SHIPS];
} GameAssets;

typedef struct {
    bool is_dragging[NUM_SHIPS];
    bool is_rotated[NUM_SHIPS];
    bool is_placed[NUM_SHIPS];
    bool is_confirmed;
    Coordinate ship_coordinates[NUM_SHIPS];
    Rectangle ship_rectangles[NUM_SHIPS];
} UiState;

static GameAssets assets;
static UiState ui_state;

typedef struct {
    int x;
    int y;
} ScreenCoord;

extern InputData input_state;

bool all_ships_placed(void) {
    for (int i = 0; i < NUM_SHIPS; i++) {
        if (!ui_state.is_placed[i]) {
            return false;
        }
    }
    return true;
}

ScreenCoord cell_coordinates(Coordinate coord, bool is_board_1) {
    int left_offset = is_board_1 ? CELL_WIDTH : (BOARD_SIZE + 2) * CELL_WIDTH;
    return (ScreenCoord){
        .x = left_offset + CELL_WIDTH * coord.x,
        .y = CELL_WIDTH + CELL_WIDTH * coord.y,
    };
}

bool in_own_board(ScreenCoord coord) {
    return CELL_WIDTH <= coord.x && coord.x < CELL_WIDTH * (BOARD_SIZE + 1) &&
           CELL_WIDTH <= coord.y && coord.y < CELL_WIDTH * (BOARD_SIZE + 1);
}

Coordinate coordinates_to_cell(ScreenCoord coord) {
    assert(in_own_board(coord));
    return (Coordinate){
        .x = (coord.x - CELL_WIDTH) / CELL_WIDTH,
        .y = (coord.y - CELL_WIDTH) / CELL_WIDTH,
    };
}

Rectangle cell_bounds(Coordinate coord, bool is_board_1) {
    ScreenCoord coords = cell_coordinates(coord, is_board_1);
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

Rectangle ship_bounds(Coordinate coord, int width, int height,
                      bool is_board_1) {
    ScreenCoord coords = cell_coordinates(coord, is_board_1);
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

    const char image_paths[NUM_SHIPS][50] = {
        "./assets/ShipBattleshipHull.png",   "./assets/ShipCarrierHull.png",
        "./assets/ShipCruiserHull.png",   "./assets/ShipSubMarineHull.png",
        "./assets/ShipDestroyerHull.png",
    };

    for (int i = 0; i < NUM_SHIPS; i++) {
        Image image = LoadImage(image_paths[i]);
        assets.textures[i] = LoadTextureFromImage(image);
        ImageRotateCCW(&image);
        assets.rotated_textures[i] = LoadTextureFromImage(image);
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
        ui_state.ship_rectangles[i] = (Rectangle){
            .x = (i + 1) * 100,
            .y = y,
            .width = assets.textures[i].width,
            .height = assets.textures[i].height,
        };
    }

    return true;
}

bool is_window_open(void) { return !WindowShouldClose(); }

void reset_ui_ships(void) {
    ui_state.is_confirmed = false;
    
    int y = 750;
    for (int i = 0; i < NUM_SHIPS; i++) {
        ui_state.is_dragging[i] = false;
        ui_state.is_placed[i] = false;
        ui_state.is_rotated[i] = false;

        ui_state.ship_rectangles[i] = (Rectangle){
            .x = (i + 1) * 100,
            .y = y,
            .width = assets.textures[i].width,
            .height = assets.textures[i].height,
        };
    }
}

static void draw_peg(Rectangle bounds, CellState cell) {
    float center_x = bounds.x + bounds.width / 2.0f;
    float center_y = bounds.y + bounds.height / 2.0f;
    float radius = bounds.width / 4.0f;

    if (cell == CELL_MISS) {
        DrawCircle(center_x, center_y, radius, RAYWHITE); 
        DrawCircleLines(center_x, center_y, radius, LIGHTGRAY);
    } else if (cell == CELL_HIT) {
        DrawCircle(center_x, center_y, radius, RED); 
        DrawCircleLines(center_x, center_y, radius, DARKGRAY);
    } else if (cell == CELL_SUNK) {
        DrawCircle(center_x, center_y, radius, MAROON); 
        DrawCircleLines(center_x, center_y, radius, BLACK);
        DrawText("X", center_x - 6, center_y - 10, 20, WHITE);
    }
}

void render_frame(const ClientState *state) {
    // Process anything currently being dragged
    bool is_any_dragging = false;
    for (int i = 0; i < NUM_SHIPS; i++) {
        if (ui_state.is_dragging[i]) {
            is_any_dragging = true;
            Rectangle *rectangle = &ui_state.ship_rectangles[i];
            
            // Move with mouse
            Vector2 mouse_delta = GetMouseDelta();
            rectangle->x += mouse_delta.x;
            rectangle->y += mouse_delta.y;

            // Handle rotation
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !ui_state.is_confirmed) {
                ui_state.is_rotated[i] = !ui_state.is_rotated[i];

                // Rotate about cursor
                Vector2 mouse = GetMousePosition();
                float offset_x = mouse.x - rectangle->x;
                float offset_y = mouse.y - rectangle->y;
                
                float temp = rectangle->width;
                rectangle->width = rectangle->height;
                rectangle->height = temp;
                
                rectangle->x = mouse.x - offset_y;
                rectangle->y = mouse.y - offset_x;
            }

            // Handle dropping and snapping
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                ui_state.is_dragging[i] = false;
                // Look at centre of top-left cell
                ScreenCoord coords = {
                    .x = rectangle->x + (CELL_WIDTH / 2),
                    .y = rectangle->y + (CELL_WIDTH / 2),
                };

                if (in_own_board(coords)) {
                    Coordinate c = coordinates_to_cell(coords);

                    // Check for tail clipping
                    int len = ship_length(i);
                    bool fits = ui_state.is_rotated[i] ? (c.x + len <= BOARD_SIZE) : (c.y + len <= BOARD_SIZE);

                    if (fits) {
                    ScreenCoord sc = cell_coordinates(c, true);
                        ui_state.ship_coordinates[i] = c;
                        ui_state.ship_rectangles[i].x = sc.x;
                        ui_state.ship_rectangles[i].y = sc.y;
                        ui_state.is_placed[i] = true;
                    } else {
                        ui_state.is_placed[i] = false; // Rejected for hanging off board
                    }
                } else {
                    ui_state.is_placed[i] = false; // Rejected for head not being on board
                }
                // TraceLog(LOG_INFO, "stopped dragging");
            }
            break; // stop looping, since at this stage we only check currently dragging ship
        }
    }

    // Now check for new interactions
    if (!is_any_dragging && !ui_state.is_confirmed) {    
        // Loop backwards to choose top most ship if any overlap
        for (int i = NUM_SHIPS - 1; i >= 0; i--) {
            Rectangle *rectangle = &ui_state.ship_rectangles[i];
            Vector2 mouse = GetMousePosition();
            
            if (CheckCollisionPointRec(mouse, *rectangle)) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    ui_state.is_dragging[i] = true;
                    ui_state.is_placed[i] = false; 
                    break; // break to prevent moving two ships at the same time
                } 
                else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                    // Rotate without dragging
                    ui_state.is_rotated[i] = !ui_state.is_rotated[i];
                    
                    float offset_x = mouse.x - rectangle->x;
                    float offset_y = mouse.y - rectangle->y;
                    
                    float temp = rectangle->width;
                    rectangle->width = rectangle->height;
                    rectangle->height = temp;
                    
                    rectangle->x = mouse.x - offset_y;
                    rectangle->y = mouse.y - offset_x;
                    
                    // TODO - DECIDE IF THIS (UNSNAPPING) IS OPTIMAL
                    ui_state.is_placed[i] = false; 
                    break; 
                }
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
            Rectangle bounds = cell_bounds((Coordinate){x, y}, true);
            if (GuiButton(bounds, "")) {
                char f[16];
                sprintf(f, "x: %d, y: %d", x, y);
                TraceLog(LOG_INFO, f);
            }
        }
    }
    GuiSetState(STATE_NORMAL);

    // Draw target grid
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Rectangle bounds = cell_bounds((Coordinate){.x = x, .y = y}, false);
            if (GuiButton(bounds, "")) {
                input_state = (InputData){
                    .grid_pos = {.x = x, .y = y},
                    .type = INPUT_FIRE,
                };
                switch (get_cell(state->game.target_board, x, y)) {
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
                case CELL_SUNK:
                case CELL_HIT:
                case CELL_MISS: // ALREADY ATTEMPTED
                    TraceLog(LOG_INFO, "Already sent a missile there.");
                    break;
                }
            } 
        }
    }

    // Draw known enemy ships
    for (int i = 0; i < NUM_SHIPS; i++) {
        if (state->game.enemy_ships_sunk[i]) {
            Position pos = state->game.enemy_ship_positions[i];
            
            // If the state says horizontal, use the base texture. Otherwise, rotated!
            Texture2D texture = pos.horizontal ? assets.rotated_textures[i] : assets.textures[i];
            
            ScreenCoord sc = cell_coordinates((Coordinate){pos.x, pos.y}, false); 
            DrawTexture(texture, sc.x, sc.y, WHITE);
        }
    }

    // Draw own ships
    for (int i = 0; i < NUM_SHIPS; i++) {
        Texture2D texture = ui_state.is_rotated[i] ? assets.rotated_textures[i]
                                                   : assets.textures[i];
        int x = ui_state.ship_rectangles[i].x;
        int y = ui_state.ship_rectangles[i].y;
        DrawTexture(texture, x, y, WHITE);
        // FOR DEBUGGING
        // DrawRectangleLinesEx(ui_state.ship_rectangles[i], 1, RED);
    }

    // Draw hit/miss pegs on both boards
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            // Check own board
            CellState my_cell = get_cell(state->game.my_board, x, y);
            if (my_cell == CELL_HIT || my_cell == CELL_SUNK || my_cell == CELL_MISS) {
                Rectangle bounds = cell_bounds((Coordinate){x, y}, true);
                draw_peg(bounds, my_cell);
            }
            
            // Check target board
            CellState target_cell = get_cell(state->game.target_board, x, y);
            if (target_cell == CELL_HIT || target_cell == CELL_SUNK || target_cell == CELL_MISS) {
                Rectangle bounds = cell_bounds((Coordinate){x, y}, false); 
                draw_peg(bounds, target_cell); 
            }
        }
    }


    if (state->current_state == UI_STATE_PLACING_SHIPS) {
        Rectangle confirm_rectangle = {
            .x = CELL_WIDTH * 9,
            .y = CELL_WIDTH * 12,
            .height = CELL_WIDTH,
            .width = CELL_WIDTH * 2,
        };
        if (!all_ships_placed()) {
            GuiSetState(STATE_DISABLED);
        }
        if (GuiButton(confirm_rectangle, "Confirm")) {
            ui_state.is_confirmed = true;
            TraceLog(LOG_INFO, "Confirmed!");
            input_state = (InputData){
                .type = INPUT_PLACED_SHIPS,
            };
            for (int i = 0; i < NUM_SHIPS; i++) {
                input_state.ships[i] =
                    (InitialShipState){.ship = i,
                                    .pos = {
                                        .x = ui_state.ship_coordinates[i].x,
                                        .y = ui_state.ship_coordinates[i].y,
                                        .horizontal = ui_state.is_rotated[i],
                                    }};
            }
        }
        if (!all_ships_placed()) {
            GuiSetState(STATE_NORMAL);
        }
    }

    const char *status_text = "";
    Color text_color = DARKGRAY;

    switch (state->current_state) {
        case UI_STATE_PLACING_SHIPS:
            status_text = ui_state.is_confirmed ? "WAITING FOR SERVER..." : "PLACE YOUR SHIPS";
            text_color = DARKBLUE;
            break;
        case UI_STATE_WAITING_FOR_OPPONENT:
            status_text = "WAITING FOR OPPONENT TO JOIN...";
            text_color = ORANGE;
            break;
        case UI_STATE_MY_TURN:
            status_text = "YOUR TURN: SELECT A TARGET!";
            text_color = DARKGREEN;
            break;
        case UI_STATE_OPPONENT_TURN:
            status_text = "OPPONENT'S TURN: BRACE FOR IMPACT!";
            text_color = MAROON;
            break;
        case UI_STATE_GAME_OVER:
            status_text = state->game.i_won ? "VICTORY! YOU WON!" : "DEFEAT! YOU LOST!";
            text_color = state->game.i_won ? GOLD : RED;
            break;
        default:
            break;
    }

    if (status_text[0] != '\0') {
        int text_width = MeasureText(status_text, FONT_SIZE);
        int center_x = (SCREEN_WIDTH - text_width) / 2;
        
        // Push the text a little lower if we are currently showing the confirm button
        int center_y = (state->current_state == UI_STATE_PLACING_SHIPS) 
                        ? CELL_WIDTH * 14 
                        : CELL_WIDTH * 12; 

        DrawText(status_text, center_x, center_y, FONT_SIZE, text_color);
    }

    EndDrawing();
}

void cleanup_graphics(void) {
    for (int i = 0; i < NUM_SHIPS; i++) {
        UnloadTexture(assets.textures[i]);
    }
    CloseWindow();
}
