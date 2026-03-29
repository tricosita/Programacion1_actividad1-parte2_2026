#pragma once

#include "raylib.h"
#include <vector>

// =============================================================================
// IMPORTANTE — orden!! 
// se inicializan en el orden en que declare ACÁ,
// no en el orden en que aparecen en el constructor. Si AnimState se declara
// después de los vectores, me queda  con valor basura aunque el constructor
// lo setee "primero". enum y m_animState van arriba.
// Also: muere
// =============================================================================

class Player
{
public:
  
    // Constructor:
  
    Player();

    
    void Init();
    void Update(float deltaTime);
    void Draw();

    void Reset();
    void SetDead();

    // -------------------------------------------------------------------------
    // mis getters
  
    float GetPosX() const { return m_posX; }
    float GetPosY() const { return m_posY; }
    int   GetAnimState() const { return (int)m_animState; }
    float GetVelY() const { return m_velY; }

private:
    // -----
    // ENUM AnimState — declarado PRIMERO en private para que se
    // inicie antes que cualquier otro que dependa de él.
    //
    // Idle: parado
    // Run:  corriendo
    // Jump: en el aire
    // Dead: muerto — unico no loop
    // ----------------------------
    enum class AnimState { Idle, Run, Jump, Dead };
    AnimState m_animState;


    // POSICIÓN Y VELOCIDAD
   
    float m_posX;
    float m_posY;
    float m_velY;

    // -----------
    // ESTADO FÍSICO Y DE CONTROL
    // onGround: true cuando el personaje está en una plataforma
    // facingRight: flip 
    // jumpCount: contador de saltos usados — máximo 2 
    
    bool m_onGround;
    bool m_facingRight;
    int  m_jumpCount;

 
    // SONIDOS
       Sound m_jumpSound;
       Sound m_deadSound;


    // ANIMACIONES
    
    std::vector<Texture2D> m_idleFrames;   // 14 frames
    std::vector<Texture2D> m_runFrames;    // 15 frames
    std::vector<Texture2D> m_jumpFrames;   // 15 frames
    std::vector<Texture2D> m_deadFrames;   // 15 frames


    // CONTROL DE ANIMACIÓN

    int   m_currentFrame;
    float m_frameTime;

    // Carga todos los frames de todas las animaciones . hacer debug mierda
    void LoadAnimations();

    // Avanza el frame de la animación activa según el tiempo transcurrido
    void UpdateAnimation(float deltaTime, bool moving);
};