#include "Player.h"
#include "Input.h"
#include "Physics.h"
#include "Collision.h"
#include "Config.h"
#include <string>

// ======
// CONSTRUCTOR
// ========
Player::Player()
{
    m_posX = 640.0f - (614.0f * 0.3f / 2.0f);
    m_posY = -200.0f;

    m_velY = 0.0f;
    m_onGround = false;

    m_currentFrame = 0;
    m_frameTime = 0.0f;

    m_animState = AnimState::Idle;
    m_facingRight = true;

    m_jumpCount = 0;
    m_jumpSound = Sound{};

    m_deadSound = Sound{};
}

//==========
// INIT
//==========

void Player::Init()
{
    LoadAnimations();
    m_jumpSound = LoadSound("assets/sounds/Cartoon Jump Sound Effect.ogg");
    // TraceLog(LOG_INFO, "jumpSound frameCount: %u", m_jumpSound.frameCount);
    m_deadSound = LoadSound("assets/sounds/Animated Fart - Sound effect.ogg");
}

// ===================
// LOADANIMATIONS
// ======================
void Player::LoadAnimations()
{
    for (int i = 1; i <= 14; i++)
        m_idleFrames.push_back(LoadTexture(("assets/img/character/Idle (" + std::to_string(i) + ").png").c_str()));

    for (int i = 1; i <= 15; i++)
        m_runFrames.push_back(LoadTexture(("assets/img/character/Run (" + std::to_string(i) + ").png").c_str()));

    for (int i = 1; i <= 15; i++)
        m_jumpFrames.push_back(LoadTexture(("assets/img/character/Jump (" + std::to_string(i) + ").png").c_str()));

    for (int i = 1; i <= 15; i++)
        m_deadFrames.push_back(LoadTexture(("assets/img/character/Dead (" + std::to_string(i) + ").png").c_str()));
}

void Player::Update(float deltaTime)
{
    /* Debug de cuando muere. borrar si vive
    static int frameCount = 0;
    frameCount++;

    if (m_animState == AnimState::Dead)
    {
        TraceLog(LOG_INFO, "DEAD en frame %d — posY: %.1f", frameCount, m_posY);
    }
    */

    InputState input = HandleInput();

    if (m_animState == AnimState::Dead)
    {
        UpdateAnimation(deltaTime, false);
        return;
    }

    if (m_posY > Config::SCREEN_HEIGHT - (564.0f * 0.3f))
    {
        m_animState = AnimState::Dead;
        m_velY = 0.0f;
        m_posY = Config::SCREEN_HEIGHT - (564.0f * 0.3f);
        PlaySound(m_deadSound);  
        UpdateAnimation(deltaTime, false);
        return;
    }

    if (input.moveRight) m_facingRight = true;
    if (input.moveLeft)  m_facingRight = false;

    if (m_posX < -60.0f)
        m_posX = -60.0f;

    if (m_posX > Config::SCREEN_WIDTH - (614.0f * 0.3f) + 60.0f)
        m_posX = Config::SCREEN_WIDTH - (614.0f * 0.3f) + 60.0f;

    if (input.moveLeft)
        m_posX -= Config::MOVE_SPEED * deltaTime;

    if (input.moveRight)
        m_posX += Config::MOVE_SPEED * deltaTime;

    if (input.jumpPressed && m_jumpCount < 2)
    {
        m_velY = Config::JUMP_FORCE;
        m_onGround = false;
        m_jumpCount++;
        PlaySound(m_jumpSound);
    }

    if (m_onGround)
        m_jumpCount = 0;

    if (!m_onGround)
        m_animState = AnimState::Jump;
    else if (input.moveLeft || input.moveRight)
        m_animState = AnimState::Run;
    else
        m_animState = AnimState::Idle;

    // Caída rápida: solo cuando ya está bajando (velY > 0), no interfiere con el salto
    if (input.fastFall && m_velY > 0)
        m_velY += Config::GRAVITY * 2.0f * deltaTime;

    ApplyPhysics(m_posY, m_velY, deltaTime);

    ResolveGroundCollision(m_posX, m_posY, m_velY, m_onGround);

    if (m_onGround)
        m_jumpCount = 0;

    UpdateAnimation(deltaTime, input.moveLeft || input.moveRight);
}

// ====================
// UPDATEANIMATION
// =====================
void Player::UpdateAnimation(float deltaTime, bool moving)
{
    const float frameSpeed = 0.1f;

    if (m_animState == AnimState::Dead)
    {
        m_frameTime += deltaTime;

        if (m_frameTime >= frameSpeed)
        {
            m_frameTime = 0.0f;

            if (m_currentFrame < (int)m_deadFrames.size() - 1)
                m_currentFrame++;
        }
        return;
    }

    m_frameTime += deltaTime;

    if (m_frameTime >= frameSpeed)
    {
        m_frameTime = 0.0f;
        m_currentFrame++;
    }

    if (!m_onGround)
        m_currentFrame %= m_jumpFrames.size();
    else if (moving)
        m_currentFrame %= m_runFrames.size();
    else
        m_currentFrame %= m_idleFrames.size();
}

    // =====================
    // forzar estado muerto desde afuera (ej: game over por bloque incorrecto)
    void Player::SetDead()
{
    m_animState    = AnimState::Dead;
    m_velY         = 0.0f;
    m_currentFrame = 0;
    m_frameTime    = 0.0f;
}

    // =============
    // volver a posicion inicial si muerto
    void Player::Reset()
{
    m_posX         = 640.0f - (614.0f * 0.3f / 2.0f);
    m_posY         = -200.0f;
    m_velY         = 0.0f;
    m_onGround     = false;
    m_currentFrame = 0;
    m_frameTime    = 0.0f;
    m_animState    = AnimState::Idle;
    m_facingRight  = true;
    m_jumpCount    = 0;
}


// =====================
// DRAW
// =====================
void Player::Draw()
{
    Texture2D texture;

    if (m_animState == AnimState::Dead)
        texture = m_deadFrames[m_currentFrame];
    else if (m_animState == AnimState::Jump)
        texture = m_jumpFrames[m_currentFrame];
    else if (m_animState == AnimState::Run)
        texture = m_runFrames[m_currentFrame];
    else
        texture = m_idleFrames[m_currentFrame];

    float scaledWidth  = texture.width * 0.3f;
    float scaledHeight = texture.height * 0.3f;

    float sourceWidth = m_facingRight ? (float)texture.width : -(float)texture.width;

    DrawTexturePro(
        texture,
        { 0, 0, sourceWidth, (float)texture.height },
        { m_posX, m_posY + 20.0f, scaledWidth, scaledHeight },
        { 0, 0 },
        0.0f,
        WHITE
    );
};