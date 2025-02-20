#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAP_WIDTH 8
#define MAP_HEIGHT 8
#define FOV 60.0f

char map[MAP_HEIGHT][MAP_WIDTH] = {{'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'},
                                   {'w', '0', '0', '0', '0', '0', '0', 'w'},
                                   {'w', '0', 'w', '0', 'w', '0', '0', 'w'},
                                   {'w', '0', '0', '0', '0', 'w', '0', 'w'},
                                   {'w', '0', 'w', '0', 'w', '0', '0', 'w'},
                                   {'w', '0', '0', 'w', '0', '0', '0', 'w'},
                                   {'w', '0', '0', '0', '0', '0', '0', 'w'},
                                   {'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'}};

typedef struct {
    Vector2 pos;
    float angle; // Only 0, 90, 180, 270
    float speed;
} Player;

bool IsPointInMap(float x, float y) {
    return x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT;
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple Raycasting FPS");
    SetTargetFPS(60);

    Player player = {.pos = {1.5f, 1.5f},
                     .angle = 0.0f, // Start facing east
                     .speed = 5.0f};

    bool showDebugMap = true;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        if (IsKeyPressed(KEY_M)) showDebugMap = !showDebugMap;

        // Movement
        float moveSpeed = player.speed * deltaTime;

        if (IsKeyDown(KEY_W)) {
            float newX = player.pos.x + cosf(player.angle * DEG2RAD) * moveSpeed;
            float newY = player.pos.y + sinf(player.angle * DEG2RAD) * moveSpeed;
            if (IsPointInMap(newX, newY) && map[(int)newY][(int)newX] != 'w') {
                player.pos.x = newX;
                player.pos.y = newY;
            }
        }
        if (IsKeyDown(KEY_S)) {
            float newX = player.pos.x - cosf(player.angle * DEG2RAD) * moveSpeed;
            float newY = player.pos.y - sinf(player.angle * DEG2RAD) * moveSpeed;
            if (IsPointInMap(newX, newY) && map[(int)newY][(int)newX] != 'w') {
                player.pos.x = newX;
                player.pos.y = newY;
            }
        }

        // Discrete 90-degree rotation on key press
        if (IsKeyPressed(KEY_A)) { // Turn left
            player.angle += 90.0f;
            if (player.angle >= 360.0f) player.angle -= 360.0f;
        }
        if (IsKeyPressed(KEY_D)) { // Turn right
            player.angle -= 90.0f;
            if (player.angle < 0.0f) player.angle += 360.0f;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        // Raycasting
        int numRays = showDebugMap ? SCREEN_WIDTH / 2 : SCREEN_WIDTH;
        float rayAngleStep = FOV / (float)numRays;

        for (int i = 0; i < numRays; i++) {
            float rayAngle = player.angle - (FOV / 2.0f) + (i * rayAngleStep);
            float rayX = player.pos.x;
            float rayY = player.pos.y;
            float rayCos = cosf(rayAngle * DEG2RAD) / 32.0f;
            float raySin = sinf(rayAngle * DEG2RAD) / 32.0f;
            float distance = 0;
            bool hitWall = false;

            while (distance < 20) {
                rayX += rayCos;
                rayY += raySin;
                distance = sqrtf(powf(rayX - player.pos.x, 2) + powf(rayY - player.pos.y, 2));

                if (!IsPointInMap(rayX, rayY)) {
                    int columnX = showDebugMap ? SCREEN_WIDTH / 2 + i * 2 : i;
                    DrawRectangle(columnX, 0, showDebugMap ? 2 : 1, SCREEN_HEIGHT, BLACK);
                    hitWall = true;
                    break;
                }

                if (map[(int)rayY][(int)rayX] == 'w') {
                    float wallHeight = (SCREEN_HEIGHT / distance) * 2;
                    int columnX = showDebugMap ? SCREEN_WIDTH / 2 + i * 2 : i;
                    DrawRectangle(columnX, SCREEN_HEIGHT / 2 - wallHeight / 2, showDebugMap ? 2 : 1, wallHeight, BLUE);
                    hitWall = true;
                    break;
                }
            }

            if (!hitWall && IsPointInMap(rayX, rayY)) {
                int columnX = showDebugMap ? SCREEN_WIDTH / 2 + i * 2 : i;
                DrawRectangle(columnX, 0, showDebugMap ? 2 : 1, SCREEN_HEIGHT, DARKGRAY);
            }
        }

        if (showDebugMap) {
            for (int y = 0; y < MAP_HEIGHT; y++) {
                for (int x = 0; x < MAP_WIDTH; x++) {
                    if (map[y][x] == 'w') {
                        DrawRectangle(x * 32, y * 32, 32, 32, GRAY);
                    }
                }
            }
            DrawCircle(player.pos.x * 32, player.pos.y * 32, 5, RED);
            DrawLine(player.pos.x * 32,
                     player.pos.y * 32,
                     player.pos.x * 32 + cosf(player.angle * DEG2RAD) * 20,
                     player.pos.y * 32 + sinf(player.angle * DEG2RAD) * 20,
                     RED);
        }

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
