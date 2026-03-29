#include "Physics.h"
#include "Config.h"

void ApplyPhysics(float& posY, float& velY, float deltaTime)
{
    // Aplicar gravedad a la velocidad vertical
    velY += Config::GRAVITY * deltaTime;

    // Integrar posición (Euler)
    posY += velY * deltaTime;
}