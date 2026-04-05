#include "Game.h"
#include "Config.h"
#include "Platforms.h"
#include <algorithm>

// =============================================================================
// CONSTRUCTOR

Game::Game()
{
    InitWindow(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, "Actividad 1 Parte 2 Triana");
    InitAudioDevice();
    SetTargetFPS(60);

    m_background        = LoadTexture("assets/img/background 5600x2800.jpg");
    m_heartFull         = LoadTexture("assets/img/vida/full heart.png");
    m_heartEmpty        = LoadTexture("assets/img/vida/empty heart.png");
    m_blockTexture      = LoadTexture("assets/img/coin.png");
    m_blockTextureUsed  = LoadTexture("assets/img/coin usada.png");

    InitBlocks();

    m_lives       = 5;
    m_gameOver    = false;
    m_gameWon     = false;
    m_playerDead  = false;
    m_prevPlayerY = 0.0f;
    m_timeLeft    = 60.0f;

    // Inicializar shake — arranca inactivo
    m_shakeTimer  = 0.0f;
    m_shakeX      = 0.0f;
    m_shakeY      = 0.0f;

    m_player.Init();
}

// =============================================================================
// RUN
// =============================================================================
void Game::Run()
{
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        Update(deltaTime);
        Draw();
    }
    CloseAudioDevice();
    CloseWindow();
}

// =============================================================================
// UPDATE
// Orden: posición anterior → jugador → bloques → colisiones →
//        partículas → shake → muerte → victoria
// =============================================================================
void Game::Update(float deltaTime)
{

    //reiniciar con R
    if (IsKeyPressed(KEY_R))
    {
        Restart();
        return;
    }

    // Guardar posición antes del update — necesario para detectar golpes desde abajo
    m_prevPlayerY = m_player.GetPosY();

    if (!m_gameWon)
        m_player.Update(deltaTime);

    // Actualizar timers de bloques (feedback visual de error)
    for (Block& b : m_blocks)
        b.Update(deltaTime);

    CheckBlockCollisions();

    // Actualizar partículas y shake — van después de colisiones
    UpdateParticles(deltaTime);
    UpdateShake(deltaTime);

    // Detectar muerte por caída
    if (m_player.GetAnimState() == 3 && !m_playerDead && !m_gameOver && !m_gameWon)
    {
        m_playerDead = true;
        m_lives--;
        if (m_lives <= 0)
            m_gameOver = true;
        else
        {
            m_player.Reset();
            m_playerDead = false;
        }
    }

    // Countdown timer — solo corre si el juego está activo
    if (!m_gameWon && !m_gameOver)
    {
        m_timeLeft -= deltaTime;
        if (m_timeLeft <= 0.0f)
        {
            m_timeLeft = 0.0f;
            m_gameOver = true;
            m_player.SetDead();
        }
    }

    // Detectar victoria — red de seguridad fuera de CheckBlockCollisions
    if (!m_gameWon && !m_gameOver && m_nextIndex >= (int)m_blocksOrdered.size())
        m_gameWon = true;

    
}

// =============================================================================
// DRAW
// Orden: fondo → bloques → partículas → jugador → HUD → instrucciones → fin
// El shake se aplica sumando m_shakeX/Y a las posiciones de destino
// =============================================================================
void Game::Draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Fondo con shake aplicado
    DrawTexturePro(
        m_background,
        { 0, 0, (float)m_background.width, (float)m_background.height },
        { m_shakeX, m_shakeY,
          (float)Config::SCREEN_WIDTH  + fabsf(m_shakeX) * 2,
          (float)Config::SCREEN_HEIGHT + fabsf(m_shakeY) * 2 },
        { 0, 0 }, 0.0f, WHITE
    );

    // Bloques con shake
    for (const Block& block : m_blocks)
        block.Draw(m_shakeX, m_shakeY);

    // Partículas — van encima de los bloques, debajo del personaje
    for (const Particle& p : m_particles)
    {
        // Fade out: alpha disminuye a medida que la partícula muere
        float alpha = p.life / p.maxLife;
        Color c     = Fade(p.color, alpha);
        DrawCircle(
            (int)(p.x + m_shakeX),
            (int)(p.y + m_shakeY),
            4.0f, c
        );
    }

    // Personaje con shake
    m_player.Draw();

    DrawHUD();

    DrawText("FLECHITAS para Moverse", 20, Config::SCREEN_HEIGHT - 60, 20, WHITE);
    DrawText("ESPACIO Saltar",         20, Config::SCREEN_HEIGHT - 35, 20, WHITE);

    // Mensajes de fin
    if (m_gameOver)
    {
        DrawText("GAME OVER",
            Config::SCREEN_WIDTH/2 - 119,
            Config::SCREEN_HEIGHT/2 - 23,
            48, BLACK);
        DrawText("GAME OVER",
            Config::SCREEN_WIDTH/2 - 120,
            Config::SCREEN_HEIGHT/2 - 24,
            48, RED);
    }
    else if (m_gameWon)
    {
        DrawText("GANASTE!",
            Config::SCREEN_WIDTH/2 - 111,
            Config::SCREEN_HEIGHT/2 - 23,
            48, BLACK);
        DrawText("GANASTE!",
            Config::SCREEN_WIDTH/2 - 112,
            Config::SCREEN_HEIGHT/2 - 24,
            48, YELLOW);
    }

    EndDrawing();
}

// =============================================================================
// DRAWHUD
// =============================================================================
void Game::DrawHUD()
{
    float heartHeight = 40.0f;
    float heartWidth  = 40.0f;
    float startX      = 20.0f;
    float startY      = 20.0f;
    float spacing     = 8.0f;

    for (int i = 0; i < 5; i++)
    {
        Texture2D& heart = (i < m_lives) ? m_heartFull : m_heartEmpty;
        DrawTexturePro(
            heart,
            { 0, 0, (float)heart.width, (float)heart.height },
            { startX + i * (heartWidth + spacing), startY, heartWidth, heartHeight },
            { 0, 0 }, 0.0f, WHITE
        );
    }

    // Timer — debajo de los corazones, mismo ancho aproximado
    int seconds   = (int)m_timeLeft;
    const char* timerText = TextFormat("%d", seconds);
    int fontSize  = 40;
    int textWidth = MeasureText(timerText, fontSize);

    // Color: blanco normal, rojo cuando quedan 10s o menos
    Color timerColor = (m_timeLeft <= 10.0f) ? RED : WHITE;

    float timerY = startY + heartHeight + 8.0f;

    // Sombra para contraste
    DrawText(timerText, (int)startX + 1, (int)timerY + 1, fontSize, BLACK);
    DrawText(timerText, (int)startX,     (int)timerY,     fontSize, timerColor);
}

// =============================================================================
// INITBLOCKS
// Genera 10 bloques sobre las 4 plataformas.
// m_blocks.reserve(10) evita realoaciones que invalidarían los punteros.
// Bloques se crean ANTES de llenar m_blocksOrdered.
// =============================================================================
void Game::InitBlocks()
{
    m_blocks.reserve(10);

    constexpr float PLAYER_HEIGHT = 564.0f * 0.3f;
    constexpr float BLOCK_SIZE    = 47.0f  * 1.0f;
    constexpr float BLOCK_OFFSET  = PLAYER_HEIGHT + BLOCK_SIZE;

    int numbers[100];
    for (int i = 0; i < 100; i++) numbers[i] = i + 1;
    for (int i = 99; i > 0; i--)
    {
        int j   = GetRandomValue(0, i);
        int tmp = numbers[i]; numbers[i] = numbers[j]; numbers[j] = tmp;
    }

    struct PlatInfo { float x, y, w; int count; };
    PlatInfo plats[] = {
        { 240, 539, 820, 4 },
        {  50, 420, 234, 2 },
        { 580, 263, 110, 2 },
        { 940, 307, 260, 2 }
    };

    int numIndex = 0;
    for (auto& p : plats)
    {
        float spacing = p.w / (p.count + 1);
        float blockY  = p.y - BLOCK_OFFSET;
        for (int i = 0; i < p.count; i++)
        {
            float blockX = p.x + spacing * (i + 1) - BLOCK_SIZE / 2.0f;
            m_blocks.emplace_back(blockX, blockY, numbers[numIndex++],
                                  m_blockTexture, m_blockTextureUsed);
        }
    }

    // Punteros ordenados — SIEMPRE después de que m_blocks esté completo
    m_blocksOrdered.clear();
    for (Block& b : m_blocks)
        m_blocksOrdered.push_back(&b);

    std::sort(m_blocksOrdered.begin(), m_blocksOrdered.end(),
        [](const Block* a, const Block* b) {
            return a->GetNumber() < b->GetNumber();
        });

    m_nextIndex = 0;
}

// =============================================================================
// CHECKBLOCKCOLLISIONS
// Golpe correcto → verde + partículas
// Golpe incorrecto → rojo 1.5s + shake + pierde vida
// =============================================================================
void Game::CheckBlockCollisions()
{
    if (m_gameOver || m_gameWon || m_nextIndex >= (int)m_blocksOrdered.size())
        return;

    // Ancho real del sprite escalado
    constexpr float SPRITE_WIDTH = 614.0f * 0.3f;   // 184px — incluye espacio vacío
    // Solo el cuerpo visual del personaje, centrado dentro del sprite
    constexpr float BODY_WIDTH   = 55.0f;
    constexpr float BODY_OFFSET  = (SPRITE_WIDTH - BODY_WIDTH) / 2.0f;

    float playerTop   = m_player.GetPosY();
    float playerLeft  = m_player.GetPosX() + BODY_OFFSET;
    float playerRight = playerLeft + BODY_WIDTH;

    for (Block& block : m_blocks)
    {
        if (!block.IsAvailable()) continue;

        Rectangle hitRect = block.GetHitRect();

        bool overlapX = playerRight > hitRect.x &&
                        playerLeft  < hitRect.x + hitRect.width;

        bool hitFromBelow = m_player.GetVelY() < 0 &&
                            m_prevPlayerY + 5.0f > hitRect.y &&
                            playerTop             <= hitRect.y + hitRect.height;

        if (overlapX && hitFromBelow)
        {
            // En lugar de comparar con ==, usar lower_bound sobre el vector ordenado
            // para encontrar la posicion del bloque golpeado y verificar que sea el esperado
            auto it = std::lower_bound(
                m_blocksOrdered.begin(), m_blocksOrdered.end(), &block,
                [](const Block* a, const Block* b) {
                    return a->GetNumber() < b->GetNumber();
                });

            bool isCorrect = (it != m_blocksOrdered.end()) &&
                             (*it == &block) &&
                             (it == m_blocksOrdered.begin() + m_nextIndex);

            if (isCorrect)
            {
                // Correcto — verde + partículas en el centro del bloque
                block.SetState(Block::State::Correct);
                SpawnParticles(block.GetPosX() + 23.5f, block.GetPosY());
                m_nextIndex++;

                if (m_nextIndex >= (int)m_blocksOrdered.size())
                    m_gameWon = true;
            }
            else
            {
                // Incorrecto — el lower_bound encontro el bloque pero no es el esperado
                // rojo + shake + pierde vida
                block.SetState(Block::State::Wrong);
                block.SetWrongTimer(1.5f);
                m_shakeTimer = 0.3f;

                m_lives--;
                // Penalidad de tiempo — clamp a 0
                m_timeLeft -= 10.0f;
                if (m_timeLeft < 0.0f) m_timeLeft = 0.0f;

                m_playerDead = true;
                if (m_lives <= 0 || m_timeLeft <= 0.0f)
                {
                    m_gameOver = true;
                    m_timeLeft = 0.0f;
                    m_player.SetDead();
                }
                else
                {
                    m_player.Reset();
                    m_playerDead = false;
                }
            }
            break;
        }
    }
}

// =============================================================================
// SPAWNPARTICLES
// Crea 8 partículas en la posición del bloque golpeado correctamente.
// Velocidad aleatoria, siempre hacia arriba, color amarillo/naranja.
// =============================================================================
void Game::SpawnParticles(float x, float y)
{
    for (int i = 0; i < 8; i++)
    {
        Particle p;
        p.x       = x;
        p.y       = y;
        p.velX    = (float)GetRandomValue(-120, 120);
        p.velY    = (float)GetRandomValue(-180, -60);  // siempre hacia arriba
        p.life    = (float)GetRandomValue(30, 60) / 100.0f;
        p.maxLife = p.life;
        p.color   = (GetRandomValue(0, 1) == 0) ? YELLOW : ORANGE;
        m_particles.push_back(p);
    }
}

// =============================================================================
// UPDATEPARTICLES
// Mueve partículas, aplica gravedad suave, elimina las que expiraron.
// Usa erase-remove idiom con lambda para limpiar el vector.
// =============================================================================
void Game::UpdateParticles(float deltaTime)
{
    for (auto& p : m_particles)
    {
        p.velY += 300.0f * deltaTime;  // gravedad suave
        p.x    += p.velX * deltaTime;
        p.y    += p.velY * deltaTime;
        p.life -= deltaTime;
    }

    // Eliminar partículas con vida <= 0
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
            [](const Particle& p) { return p.life <= 0.0f; }),
        m_particles.end()
    );
}

// =============================================================================
// UPDATESHAKE
// Descuenta el timer y genera offsets aleatorios mientras está activo.
// Cuando expira, resetea los offsets a 0 para que todo vuelva a su lugar.
// =============================================================================
void Game::UpdateShake(float deltaTime)
{
    if (m_shakeTimer > 0.0f)
    {
        m_shakeTimer -= deltaTime;
        m_shakeX      = (float)GetRandomValue(-6, 6);
        m_shakeY      = (float)GetRandomValue(-6, 6);
    }
    else
    {
        m_shakeTimer = 0.0f;
        m_shakeX     = 0.0f;
        m_shakeY     = 0.0f;
    }
}

// =============================================================================
// RESTART
// Reinicia el estado completo del juego sin cerrar la ventana.
// Regenera los bloques con números aleatorios nuevos.
// =============================================================================
void Game::Restart()
{
    // Resetear estado del juego
    m_lives      = 5;
    m_gameOver   = false;
    m_gameWon    = false;
    m_playerDead = false;
    m_timeLeft   = 60.0f;
    m_shakeTimer = 0.0f;
    m_shakeX     = 0.0f;
    m_shakeY     = 0.0f;

    // Limpiar partículas y bloques
    m_particles.clear();
    m_blocks.clear();
    m_blocksOrdered.clear();

    // Regenerar bloques con números nuevos
    InitBlocks();

    // Resetear personaje a posición inicial
    m_player.Reset();
}