#include "Collision.h"
#include "Platforms.h"
#include "Config.h"

void ResolveGroundCollision(float& posX, float& posY, float& velY, bool& onGround)
{
    constexpr float SPRITE_WIDTH  = 614.0f * 0.3f;
    constexpr float SPRITE_HEIGHT = 564.0f * 0.3f;

    float feetY = posY + SPRITE_HEIGHT;
    onGround = false;

    for (const Rectangle& plat : GetPlatforms())
    {
        bool overlapX = (posX + SPRITE_WIDTH > plat.x) &&
                        (posX < plat.x + plat.width);

        /* Debug temporal
        TraceLog(LOG_INFO, "plat.y=%.0f feetY=%.1f overlapX=%d velY=%.1f", 
                 plat.y, feetY, overlapX, velY);*/

        if (overlapX && velY >= 0 && feetY >= plat.y && feetY <= plat.y + 60.0f)
        {
            posY     = plat.y - SPRITE_HEIGHT;
            velY     = 0.0f;
            onGround = true;
            return;
        }
    }
}