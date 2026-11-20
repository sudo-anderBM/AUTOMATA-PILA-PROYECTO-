#include "raylib.h"
#include <string>

int main() 
{
    InitWindow(1000, 700, "Automata de pila");
    SetTargetFPS(60);

    while (!WindowShouldClose()) 
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("palindormos pares", 20,20,24,DARKBLUE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
