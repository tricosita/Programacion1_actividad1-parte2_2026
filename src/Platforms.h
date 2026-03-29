#pragma once

#include "raylib.h"
#include <vector>


// Solo el borde superior (rect.y) actúa como colisionador
inline const std::vector<Rectangle> GetPlatforms()
{
    return {
        { 240, 539, 800, 20 },   // suelo principal
        { 50, 420, 200, 20 },   // izquierda
        { 580, 263, 90, 20 },   // centro
        { 1000, 307, 220, 20 }    // derecha
    };
}
