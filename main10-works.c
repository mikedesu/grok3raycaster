#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAP_WIDTH 8
#define MAP_HEIGHT 8
#define FOV 60.0f
#define CELL_SIZE 1.0f
#define PLAYER_OFFSET 0.5f

char map[MAP_HEIGHT][MAP_WIDTH] = {{'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'},
                                   {'w', '0', '0', '0', '0', '0', '0', 'w'},
                                   {'w', '0', 'w', '0', 'w', '0', '0', 'w'},
                                   {'w', '0', '0', '0', '0', 'w', '0', 'w'},
                                   {'w', '0', 'w', '0', 'w', '0', '0', 'w'},
                                   {'w', '0', '0', 'w', '0', '0', '0', 'w'},
                                   {'w', '0', '0', '0', '0', '0', '0', 'w'},
                                   {'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'}};

typedef struct {
    Vector2 pos; // Integer grid position (0, 1, 2, etc.)
    float angle; // Only 0, 90, 180, 270
    float speed; // Unused, kept for potential future use
} Player;

bool IsPointInMap(float x, float y) {
    return x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT;
}

void GetMovementDirections(float angle, float* forwardX, float* forwardY, float* backwardX, float* backwardY) {
    // Define forward and backward directions based on angle
    if (angle == 0.0f) { // East
        *forwardX = 1.0f;
        *forwardY = 0.0f;
        *backwardX = -1.0f;
        *backwardY = 0.0f;
    } else if (angle == 90.0f) { // North (up, -Y)
        *forwardX = 0.0f;
        *forwardY = 1.0f;
        *backwardX = 0.0f;
        *backwardY = -1.0f;
    } else if (angle == 180.0f) { // West
        *forwardX = -1.0f;
        *forwardY = 0.0f;
        *backwardX = 1.0f;
        *backwardY = 0.0f;
    } else if (angle == 270.0f) { // South (down, +Y)
        *forwardX = 0.0f;
        *forwardY = -1.0f;
        *backwardX = 0.0f;
        *backwardY = 1.0f;
    }
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple Raycasting FPS");
    SetTargetFPS(60);

    Player player = {
        .pos = {1.0f, 1.0f}, // Start at grid position (1, 1)
        .angle = 0.0f, // Start facing east
        .speed = 5.0f // Unused
    };

    bool showDebugMap = true;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        if (IsKeyPressed(KEY_M)) showDebugMap = !showDebugMap;

        // Grid-based movement (1.0 unit steps)
        float forwardX, forwardY, backwardX, backwardY;
        GetMovementDirections(player.angle, &forwardX, &forwardY, &backwardX, &backwardY);

        if (IsKeyPressed(KEY_W)) { // Move forward
            float newX = player.pos.x + forwardX * CELL_SIZE;
            float newY = player.pos.y + forwardY * CELL_SIZE;
            if (IsPointInMap(newX, newY) && map[(int)newY][(int)newX] != 'w') {
                player.pos.x = newX;
                player.pos.y = newY;
            }
        }
        if (IsKeyPressed(KEY_S)) { // Move backward
            float newX = player.pos.x + backwardX * CELL_SIZE;
            float newY = player.pos.y + backwardY * CELL_SIZE;
            if (IsPointInMap(newX, newY) && map[(int)newY][(int)newX] != 'w') {
                player.pos.x = newX;
                player.pos.y = newY;
            }
        }

        // Discrete 90-degree rotation
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

        // Raycasting with offset
        int numRays = showDebugMap ? SCREEN_WIDTH / 2 : SCREEN_WIDTH;
        float rayAngleStep = FOV / (float)numRays;

        for (int i = 0; i < numRays; i++) {
            float rayAngle = player.angle - (FOV / 2.0f) + (i * rayAngleStep);
            float rayX = player.pos.x + PLAYER_OFFSET;
            float rayY = player.pos.y + PLAYER_OFFSET;
            float rayCos = cosf(rayAngle * DEG2RAD) / 32.0f;
            float raySin = sinf(rayAngle * DEG2RAD) / 32.0f;
            float distance = 0;
            bool hitWall = false;

            while (distance < 20) {
                rayX += rayCos;
                rayY += raySin;
                distance = sqrtf(powf(rayX - (player.pos.x + PLAYER_OFFSET), 2) +
                                 powf(rayY - (player.pos.y + PLAYER_OFFSET), 2));

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
            DrawCircle((player.pos.x + PLAYER_OFFSET) * 32, (player.pos.y + PLAYER_OFFSET) * 32, 5, RED);
            DrawLine((player.pos.x + PLAYER_OFFSET) * 32,
                     (player.pos.y + PLAYER_OFFSET) * 32,
                     (player.pos.x + PLAYER_OFFSET) * 32 + cosf(player.angle * DEG2RAD) * 20,
                     (player.pos.y + PLAYER_OFFSET) * 32 + sinf(player.angle * DEG2RAD) * 20,
                     RED);
        }

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
