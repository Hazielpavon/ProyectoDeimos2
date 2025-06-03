// entidad.cpp
#include "entidad.h"
#include <QDebug>

entidad::entidad()
    : m_transformacion()
    , m_sprite()
    , m_componenteFisico(&m_transformacion)
    , m_componenteSalud()
    , m_isJumping(false)
    , m_isAttacking(false)
    , m_verticalVelocity(0.0f)
    , m_groundY(0.0f)
    , m_facingLeft(false)
{
    // 1) Animación “Slashing” (golpe)
    m_sprite.loadFrames(SpriteState::Slashing,      ":/resources/0_Blood_Demon_Slashing_",     12);
    m_sprite.generateMirroredFrames(SpriteState::Slashing, SpriteState::Slashingleft);

    // 2) Animación “Walking” / “WalkingLeft”
    m_sprite.loadFrames(SpriteState::Walking,       ":/resources/0_Blood_Demon_Walking_",      24);
    m_sprite.loadFrames(SpriteState::WalkingLeft,   ":/resources/0_Blood_Demon_WalkingL_",     24);

    // 3) Animación “Idle” / “IdleLeft”
    m_sprite.loadFrames(SpriteState::Idle,          ":/resources/0_Blood_Demon_Idle_",         16);
    m_sprite.loadFrames(SpriteState::IdleLeft,      ":/resources/0_Blood_Demon_IdleL_",        16);

    // 4) Animación “Jump” / “JumpLeft”
    m_sprite.loadFrames(SpriteState::Jump,          ":/resources/0_Blood_Demon_Jump Loop_",     6);
    m_sprite.generateMirroredFrames(SpriteState::Jump, SpriteState::JumpLeft);

    // 5) Animación “Running” / “RunningLeft”
    m_sprite.loadFrames(SpriteState::Running,       ":/resources/0_Blood_Demon_Running_",      12);
    m_sprite.generateMirroredFrames(SpriteState::Running, SpriteState::RunningLeft);

    // 6) Animación “Sliding” / “SlidingLeft”
    m_sprite.loadFrames(SpriteState::Sliding,       ":/resources/0_Blood_Demon_Sliding_",       6);
    m_sprite.generateMirroredFrames(SpriteState::Sliding, SpriteState::SlidingLeft);

    // Ajustes generales (FPS + tamaño de dibujo)
    m_sprite.setFPS(12);
    m_sprite.setSize(128, 128);

    // Estado inicial es Idle
    m_sprite.setState(SpriteState::Idle);
    m_componenteSalud.setHP(100);
}

entidad::~entidad()
{
    // Nada especial
}

void entidad::startJump()
{
    // No permitimos saltar si ya estamos en salto o ya atacando
    if (m_isJumping || m_isAttacking) return;

    m_groundY = m_transformacion.getPosition().y();
    m_verticalVelocity = -JUMP_VELOCITY;
    m_isJumping = true;

    if (m_facingLeft) {
        m_sprite.setState(SpriteState::JumpLeft);
    } else {
        m_sprite.setState(SpriteState::Jump);
    }
}

void entidad::startAttack()
{
    // Si ya está saltando o ya en medio de ataque, no hacemos nada
    if (m_isJumping || m_isAttacking) return;

    m_isAttacking = true;
    if (m_facingLeft) {
        m_sprite.setState(SpriteState::Slashingleft);
    } else {
        m_sprite.setState(SpriteState::Slashing);
    }
}

void entidad::actualizar(float dt)
{
    // Física de salto
    if (m_isJumping) {
        actualizarSalto(dt);
    }

    // Actualizar posición según velocidad
    m_componenteFisico.actualizar(dt);

    // Ajustar posición de dibujo (centrar el sprite)
    QPointF posF = m_transformacion.getPosition();
    int w = m_sprite.getSize().width();
    int h = m_sprite.getSize().height();
    m_sprite.setPosition(int(posF.x() - w/2.0f),
                         int(posF.y() - h/2.0f));

    // Si está saltando → solo advance frame de salto
    if (m_isJumping) {
        m_sprite.update(dt);
    }
    else {
        // Si no está saltando → actualizar según estado (including attack)
        actualizarAnimacion(dt);
    }

    // Actualizar salud, timers, etc.
    m_componenteSalud.actualizar(dt);
}


void entidad::actualizarSalto(float dt)
{
    QPointF pos = m_transformacion.getPosition();
    pos.setY(pos.y() + m_verticalVelocity * dt);
    m_verticalVelocity += GRAVITY * dt;

    // Si toca el suelo, detenemos salto
    if (pos.y() >= m_groundY) {
        pos.setY(m_groundY);
        m_verticalVelocity = 0.0f;
        m_isJumping = false;

        // Al aterrizar, regresamos a Idle (mirando a la dirección que corresponda)
        if (m_facingLeft) {
            m_sprite.setState(SpriteState::IdleLeft);
        } else {
            m_sprite.setState(SpriteState::Idle);
        }
    }
    m_transformacion.setPosition(pos.x(), pos.y());
}

void entidad::actualizarAnimacion(float dt)
{
    float vx = m_componenteFisico.velocity().x();
    SpriteState estadoActual = m_sprite.getState();

    // 1) Si estamos en medio de un ataque, avanzamos sus fotogramas:
    if (estadoActual == SpriteState::Slashing
        || estadoActual == SpriteState::Slashingleft)
    {
        // Avanzar un fotograma de la animación de “slashing”:
        m_sprite.update(dt);

        // Obtenemos el vector de QPixmap de ese estado:
        const QVector<QPixmap> &framesAtk = m_sprite.framesForState(estadoActual);

        // Control para asegurarnos de recorrer todos los fotogramas antes de volver a suelo
        static bool hasReachedLastFrame = false;

        // Si aún no habíamos llegado al último fotograma, pero ahora sí:
        if (!hasReachedLastFrame &&
            m_sprite.currentFrameIndex() == framesAtk.size() - 1)
        {
            hasReachedLastFrame = true;
        }

        // Una vez que llegamos al último fotograma y volvemos a índice 0,
        // marcamos que se terminó el ataque y regresamos a animación de suelo:
        if (hasReachedLastFrame && m_sprite.currentFrameIndex() == 0)
        {
            hasReachedLastFrame = false;
            m_isAttacking = false;

            // Al terminar el ataque, volvemos a Idle/Walking/Running según vx:
            if (qFuzzyCompare(vx, 0.0f)) {
                if (m_facingLeft) m_sprite.setState(SpriteState::IdleLeft);
                else              m_sprite.setState(SpriteState::Idle);
            }
            else if (vx < 0.0f) {
                m_facingLeft = true;
                const float runThreshold = 160.0f;
                if (qAbs(vx) > runThreshold)
                    m_sprite.setState(SpriteState::RunningLeft);
                else
                    m_sprite.setState(SpriteState::WalkingLeft);
            }
            else {
                m_facingLeft = false;
                const float runThreshold = 160.0f;
                if (vx > runThreshold)
                    m_sprite.setState(SpriteState::Running);
                else
                    m_sprite.setState(SpriteState::Walking);
            }
        }

        // Ya hemos avanzado un fotograma de ataque, no hacemos nada más:
        return;
    }

    // 2) Si estamos en salto, avanzamos ese fotograma y salimos:
    if (m_isJumping) {
        m_sprite.update(dt);
        return;
    }

    // 3) Si no estamos atacando ni saltando → animaciones de suelo (Idle/Walking/Running):
    if (qFuzzyCompare(vx, 0.0f)) {
        if (m_facingLeft) m_sprite.setState(SpriteState::IdleLeft);
        else              m_sprite.setState(SpriteState::Idle);
    }
    else if (vx < 0.0f) {
        m_facingLeft = true;
        const float runThreshold = 160.0f;
        if (qAbs(vx) > runThreshold)
            m_sprite.setState(SpriteState::RunningLeft);
        else
            m_sprite.setState(SpriteState::WalkingLeft);
    }
    else { // vx > 0
        m_facingLeft = false;
        const float runThreshold = 160.0f;
        if (vx > runThreshold)
            m_sprite.setState(SpriteState::Running);
        else
            m_sprite.setState(SpriteState::Walking);
    }

    // Avanzar el fotograma de la animación elegida:
    m_sprite.update(dt);
}

