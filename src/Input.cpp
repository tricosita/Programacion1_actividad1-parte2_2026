#include "Input.h"
#include "raylib.h"

InputState HandleInput()
{
    InputState input;

    // Detectar movimiento continuo
    input.moveLeft = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    input.moveRight = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);

    // Detectar salto (evento único)
    input.jumpPressed = IsKeyPressed(KEY_SPACE);

    // Caída rápida — solo tiene efecto si está en el aire
    input.fastFall = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);

    return input;
}