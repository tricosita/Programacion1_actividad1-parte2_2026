#pragma once

#include "Player.h"
#include "Block.h"
#include "raylib.h"
#include <vector>

class Game
{
public:
    Game();
    void Run();

private:

    //reiniciar
    void Restart();

    
    // Jugador y fondo
    Player    m_player;
    Texture2D m_background;

    // HUD  vidas
    Texture2D m_heartFull;
    Texture2D m_heartEmpty;
    Texture2D m_blockTextureUsed;
    int       m_lives;
    bool      m_gameOver;
    bool      m_playerDead;
    bool m_gameWon;

    //====================
    // Bloques
    Texture2D          m_blockTexture;
    std::vector<Block> m_blocks;
    std::vector<Block*> m_blocksOrdered;  // punteros ordenados por número
    int     m_nextIndex;       // índice del próximo bloque esperado
    float   m_prevPlayerY;     // posición Y del personaje en el frame anterior

    // magia negra bloques
    void Update(float deltaTime);
    void Draw();
    void DrawHUD();
    void InitBlocks();   // los 10 bloques
    void CheckBlockCollisions();  // detecta golpes 

    //=====================
    //  PARTÍCULAS 
    struct Particle 
    {
    float x, y;       // posición actual
    float velX, velY; // velocidad en px/s
    float life;       // tiempo de vida restante en segundos
    float maxLife;    // vida inicial — para calcular alpha de fade
    Color color;      // color de la partícula
    };
    
    std::vector<Particle> m_particles;

    //=====================
    // sacudon error
    float m_shakeTimer;   // tiempo restante de temblor
    float m_shakeX;       // offset X actual
    float m_shakeY;       // offset Y actual

    // Métodos
    void SpawnParticles(float x, float y);
    void UpdateParticles(float deltaTime);
    void UpdateShake(float deltaTime);
    
};