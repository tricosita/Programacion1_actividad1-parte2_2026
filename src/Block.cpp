#include "Block.h"
#include <cmath> //para el sinf de movimiento de blouques

// Tamaño original del sprite: 47x47
static constexpr float SPRITE_SIZE = 47.0f;
static constexpr float SCALED_SIZE = SPRITE_SIZE * 1.0f;  

// ================================
// CONSTRUCTOR

Block::Block(float x, float y, int number, Texture2D texture, Texture2D textureUsed)
    : m_posX(x), m_posY(y), m_number(number),
      m_state(State::Untouched), m_texture(texture),
      m_textureUsed(textureUsed), m_wrongTimer(0.0f),
      m_floatTimer(0.0f),
      m_floatPhase((float)(number * 1.3f))  // fase distinta por número
{}

// ==================
// UPDATE
//Si el bloque está en estado Wrong, descuenta el timer.

void Block::Update(float deltaTime)
{
    // Timer de flotación — avanza siempre
    m_floatTimer += deltaTime;

    // Timer de error
    if (m_state == State::Wrong)
    {
        m_wrongTimer -= deltaTime;
        if (m_wrongTimer <= 0.0f)
        {
            m_state      = State::Untouched;
            m_wrongTimer = 0.0f;
        }
    }
}

// ===============================
// ISAVAILABLE
// Devuelve true solo si el bloque está en estado Untouched.

bool Block::IsAvailable() const
{
    return m_state == State::Untouched;
}

// =============================================================================
// DRAW
// Recibe shakeX y shakeY desde Game para sincronizar el temblor de pantalla.
// El floatOffset hace que cada bloque flote independientemente (fase distinta).
// La textura cambia a coin usada cuando el estado es Correct.
// El número desaparece cuando el bloque fue golpeado correctamente.
// =============================================================================

void Block::Draw(float shakeX, float shakeY) const
{
    // Offset vertical senoidal — 4px de amplitud, frecuencia 2Hz
    // m_floatPhase es distinta por bloque para evitar que floten sincronizados
    float floatOffset = sinf(m_floatTimer * 2.0f + m_floatPhase) * 4.0f;

    // Correct → coin usada, cualquier otro estado → coin normal
    const Texture2D& tex = (m_state == State::Correct) ? m_textureUsed : m_texture;

    // Dibujar sprite con flotación y shake aplicados
    DrawTexturePro(
        tex,
        { 0, 0, SPRITE_SIZE, SPRITE_SIZE },
        { m_posX + shakeX, m_posY + floatOffset + shakeY, SCALED_SIZE, SCALED_SIZE },
        { 0, 0 }, 0.0f, WHITE
    );

    // No mostrar número si el bloque fue golpeado correctamente
    if (m_state == State::Correct) return;

    // Color del número según estado
    Color textColor;
    switch (m_state)
    {
        case State::Wrong: textColor = RED;   break;
        default:           textColor = WHITE; break;
    }

    // Número centrado horizontalmente sobre el bloque, con flotación y shake
    const char* numText   = TextFormat("%d", m_number);
    int         textWidth = MeasureText(numText, 16);
    float       textX     = m_posX + shakeX + (SCALED_SIZE / 2.0f) - (textWidth / 2.0f);
    float       textY     = m_posY + shakeY + floatOffset - 20.0f;

    // Sombra para legibilidad sobre cualquier fondo
    DrawText(numText, (int)textX + 1, (int)textY + 1, 16, BLACK);
    DrawText(numText, (int)textX,     (int)textY,     16, textColor);
}

// ==========================================
// GETHITRECT
// Devuelve el rectángulo de colisión — franja central de la base del bloque.
//margen de 03 sigue siendo grande
// Para ajustar la precisión: más alto = área más chica.
// ====================================
Rectangle Block::GetHitRect() const
{
    // margin = 0.5 daba ancho 0 (47 - 47 = 0). Con 0.15 queda ~33px de ancho útil.
    float margin = SCALED_SIZE * 0.15f;
    return {
        m_posX + margin,
        m_posY + SCALED_SIZE - 4.0f,
        SCALED_SIZE - margin * 2.0f,
        4.0f
    };
}

// =========================================
// SETSTATE
// Cambia el estado visible del bloque.
// Para  Wrong, usar SetWrongTimer() después para activar el timer.
// =========================
void Block::SetState(State state)
{
    m_state = state;
}

// =====================================================
// SETWRONGTIMER
// Activa el timer de feedback visual para golpe incorrecto.
// Después de `seconds` segundos, Update() resetea el bloque a Untouched.

void Block::SetWrongTimer(float seconds)
{
    m_wrongTimer = seconds;
}