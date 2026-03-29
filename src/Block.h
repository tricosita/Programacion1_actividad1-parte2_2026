#pragma once
#include "raylib.h"

// =============================================================================
// CLASE: Block
// Representa un bloque individual con número visible.
// Estado determina el color del número mostrado.
// =============================================================================
class Block
{
public:
    // Estado visible del bloque
    enum class State { Untouched, Correct, Wrong };

    // Constructor: posición y número asignado
    Block(float x, float y, int number, Texture2D texture, Texture2D textureUsed);

    // Dibuja el sprite y el número flotando encima
    void Draw() const;

    // Devuelve el rectángulo de la BASE del bloque (para detección de golpe desde abajo)
    Rectangle GetHitRect() const;

    // Cambia el estado — afecta el color del número
    void SetState(State state);

    // reseteo de bloque si toque mal
    void SetWrongTimer(float seconds); //para que quede rojo un raitto
    void Reset() { m_state = State::Untouched; }
    void Update(float deltaTime);
    bool IsAvailable() const;  // true si se puede golpear

    //shake
   void Draw(float shakeX = 0.0f, float shakeY = 0.0f) const;



    // Getters
    int   GetNumber() const { return m_number; }
    State GetState()  const { return m_state;  }

    //Para el spaneo de partículas
    float GetPosX() const { return m_posX; }
    float GetPosY() const { return m_posY; }

        // Posición del bloque en pantalla
    float     m_posX;
    float     m_posY;

private:


    
    
    int       m_number;
    State     m_state;
    Texture2D m_texture;
    Texture2D m_textureUsed;  // textura cuando está en estado Correct
    float     m_floatTimer;   // acumula tiempo para la animación de flotación
    float     m_floatPhase;   // fase inicial — para que no floten sincronizados

    // Tamaño del bloque en pantalla (50% del original)
    static constexpr float SCALE = 0.5f;

    float m_wrongTimer;  // tiempo restante mostrando rojo
};