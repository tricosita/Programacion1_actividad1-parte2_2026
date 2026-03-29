#pragma once

// Definir estructura de intención del jugador
struct InputState
{
    bool moveLeft = false;
    bool moveRight = false;
    bool jumpPressed = false;
    bool fastFall = false;
};

// Declarar función de lectura de input
InputState HandleInput();