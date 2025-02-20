#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAP_WIDTH 8
#define MAP_HEIGHT 8
#define FOV 60.0f // Field of view in degrees

// Simple map using chars: 0 = empty, w = wall
char map[MAP_HEIGHT][MAP_WIDTH] = {{'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'},
                                   {'w', '0', '0', '0', '0', '0', '0', 'w'},
                                   {'w', '0', 'w', '0', 'w', '0', '0', 'w'},
                                   {'w', '0', '0', '0', '0', 'w', '0', 'w'},
                                   {'w', '0', 'w', '0', 'w', '0', '0', 'w'},
                                   {'w', '0', '0', 'w', '0', '0', '0', 'w'},
                                   {'w', '0', '0', '0', '0', '0', '0', 'w'},
                                   {'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'}};

typedef struct {
    Vector2 pos; // Player position
    float angle; // Player direction in degrees
    float speed; // Movement speed
} Player;

int main(void) {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple Raycasting FPS");
    SetTargetFPS(60);

    // Initialize player
    Player player = {.pos = {2.5f, 2.5f}, // Start in middle of empty space
                     .angle = 0.0f,
                     .speed = 5.0f};

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        float deltaTime = GetFrameTime();

        // Player movement
        float moveSpeed = player.speed * deltaTime;
        float rotSpeed = 90.0f * deltaTime; // 90 degrees per second

        // Forward/Backward movement
        if (IsKeyDown(KEY_W)) {
            float newX = player.pos.x + cosf(player.angle * DEG2RAD) * moveSpeed;
            float newY = player.pos.y + sinf(player.angle * DEG2RAD) * moveSpeed;
            //if (map[(int)newY][(int)newX] != 'w') {
            player.pos.x = newX;
            player.pos.y = newY;
            //}
        }
        if (IsKeyDown(KEY_S)) {
            float newX = player.pos.x - cosf(player.angle * DEG2RAD) * moveSpeed;
            float newY = player.pos.y - sinf(player.angle * DEG2RAD) * moveSpeed;
            //if (map[(int)newY][(int)newX] != 'w') {
            player.pos.x = newX;
            player.pos.y = newY;
            //}
        }

        // Rotation
        if (IsKeyDown(KEY_A)) player.angle -= rotSpeed;
        if (IsKeyDown(KEY_D)) player.angle += rotSpeed;

        // Keep angle between 0 and 360
        if (player.angle >= 360.0f) player.angle -= 360.0f;
        if (player.angle < 0.0f) player.angle += 360.0f;

        // Begin drawing
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw 2D map (for debugging)
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                if (map[y][x] == 'w') {
                    DrawRectangle(x * 32, y * 32, 32, 32, GRAY);
                }
            }
        }

        // Draw player on map (for debugging)
        DrawCircle(player.pos.x * 32, player.pos.y * 32, 5, RED);
        DrawLine(player.pos.x * 32,
                 player.pos.y * 32,
                 player.pos.x * 32 + cosf(player.angle * DEG2RAD) * 20,
                 player.pos.y * 32 + sinf(player.angle * DEG2RAD) * 20,
                 RED);

        // Basic raycasting (to be expanded)
        int numRays = SCREEN_WIDTH / 2;
        float rayAngleStep = FOV / (float)numRays;

        for (int i = 0; i < numRays; i++) {
            float rayAngle = player.angle - (FOV / 2.0f) + (i * rayAngleStep);
            float rayX = player.pos.x;
            float rayY = player.pos.y;
            float rayCos = cosf(rayAngle * DEG2RAD) / 32.0f;
            float raySin = sinf(rayAngle * DEG2RAD) / 32.0f;
            float distance = 0;

            // Simple raycasting loop
            while (distance < 20) { // Max distance
                rayX += rayCos;
                rayY += raySin;
                distance = sqrtf(powf(rayX - player.pos.x, 2) + powf(rayY - player.pos.y, 2));

                if (map[(int)rayY][(int)rayX] == 'w') {
                    // Calculate wall height based on distance
                    float wallHeight = (SCREEN_HEIGHT / distance) * 2;
                    int columnX = SCREEN_WIDTH / 2 + i * 2;
                    DrawRectangle(columnX, SCREEN_HEIGHT / 2 - wallHeight / 2, 2, wallHeight, BLUE);
                    break;
                }
            }
        }

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
