#include <stdio.h>

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main(void) {
    printf("Hello, World!\n");

    // 1. Initialize the window (Width, Height, Title)
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "raylib - Hello World");

    // 2. Target 60 frames per second
    SetTargetFPS(60);

    // 3. Main game loop
    // WindowShouldClose() checks if the ESC key or the window 'X' button was pressed
    while (!WindowShouldClose()) {

        // 4. Update logic goes here (if any)

        // 5. Drawing logic
        BeginDrawing();

            // Clear the screen with a background color (RAYWHITE is built-in)
            ClearBackground(RAYWHITE);

            // Draw text: ("Text", X position, Y position, Font Size, Color)
            // Measuring text width helps us center it perfectly
            // int textWidth = MeasureText("Hello, World!", 40);
            DrawText("Hello, World!", 0, 0, 40, DARKGRAY);

            // Draw a raygui button!
            // GuiButton takes a Rectangle and a label string.
            // It automatically handles hover states, clicks, and returns true when pressed.
            if (GuiButton((Rectangle){ 300, 200, 200, 50 }, "Click Me!")) {
                TraceLog(LOG_INFO, "Raygui Button Clicked!");
            }

        EndDrawing();
    }

    // 6. De-initialize and close the window, freeing up system resources
    CloseWindow();

    return 0;
}
