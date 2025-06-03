// entidad.cpp
#include "entidad.h"
#include <QDebug>

entidad::entidad()
    : m_transformacion()
    , m_sprite()
    , m_componenteFisico(&m_transformacion)
    , m_componenteSalud()
    , m_isJumping(false)
    , m_verticalVelocity(0.0f)
    , m_groundY(0.0f)
    , m_facingLeft(false)
{
    // 1) Carga de animaciones básicas
    m_sprite.loadFrames(SpriteState::Sliding,":/resources/0_Blood_Demon_Sliding_",6);
    m_sprite.generateMirroredFrames(SpriteState::Sliding,SpriteState::SlidingLeft);

    m_sprite.loadFrames(SpriteState::Walking,":/resources/0_Blood_Demon_Walking_",  24);
    m_sprite.loadFrames(SpriteState::Idle,":/resources/0_Blood_Demon_Idle_",     16);
    m_sprite.loadFrames(SpriteState::IdleLeft,":/resources/0_Blood_Demon_IdleL_",    16);
    m_sprite.loadFrames(SpriteState::WalkingLeft, ":/resources/0_Blood_Demon_WalkingL_", 24);

    // 2) Animación de salto (loop)
    m_sprite.loadFrames(SpriteState::Jump,":/resources/0_Blood_Demon_Jump Loop_",   6);
    m_sprite.generateMirroredFrames(SpriteState::Jump,      SpriteState::JumpLeft);

    // 3) Animación de correr
    m_sprite.loadFrames(SpriteState::Running,":/resources/0_Blood_Demon_Running_",  12);
    m_sprite.generateMirroredFrames(SpriteState::Running,   SpriteState::RunningLeft);

    // 4) Animación de deslizar (sliding)


    // Ajustes generales (fps y tamaño)
    m_sprite.setFPS(12);
    m_sprite.setSize(128, 128);

    // Estado inicial
    m_sprite.setState(SpriteState::Idle);
    m_componenteSalud.setHP(100);
}

entidad::~entidad()
{
    // Nada especial
}

void entidad::startJump()
{
    if (m_isJumping) return;  // no hacer doble salto

    m_groundY = m_transformacion.getPosition().y();
    m_verticalVelocity = -JUMP_VELOCITY;
    m_isJumping = true;

    if (m_facingLeft) {
        m_sprite.setState(SpriteState::JumpLeft);
    } else {
        m_sprite.setState(SpriteState::Jump);
    }
}

void entidad::actualizar(float dt)
{
    // 1) Si estamos en salto, actualizamos la física del salto
    if (m_isJumping) {
        actualizarSalto(dt);
    }

    // 2) Actualizamos el componente físico (posición XY)
    m_componenteFisico.actualizar(dt);

    // 3) Ajustamos la posición de dibujo centrándola:
    QPointF posF = m_transformacion.getPosition();
    int spriteW = m_sprite.getSize().width();
    int spriteH = m_sprite.getSize().height();
    int drawX = int(posF.x() - (spriteW * 0.5f));
    int drawY = int(posF.y() - (spriteH * 0.5f));
    m_sprite.setPosition(drawX, drawY);

    // 4) Si seguimos saltando, solo actualizamos el frame de salto:
    if (m_isJumping) {
        m_sprite.update(dt);
    }
    else {
        // 5) Si no estamos en el aire, actualizamos la animación de suelo
        actualizarAnimacion(dt);
    }

    // 6) Salud (si quieres dibujar barra, etc.)
    m_componenteSalud.actualizar(dt);
}

void entidad::actualizarSalto(float dt)
{
    QPointF pos = m_transformacion.getPosition();
    pos.setY(pos.y() + m_verticalVelocity * dt);
    m_verticalVelocity += GRAVITY * dt;

    // Al llegar o pasar el suelo, detenemos salto
    if (pos.y() >= m_groundY) {
        pos.setY(m_groundY);
        m_verticalVelocity = 0.0f;
        m_isJumping = false;

        // Al aterrizar, volvemos a Idle (mirando a izquierda o derecha)
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

    // 1) Si ya estamos en Sliding o SlidingLeft, avanzamos sus frames y salimos
    SpriteState estadoActual = m_sprite.getState();
    if (estadoActual == SpriteState::Sliding || estadoActual == SpriteState::SlidingLeft) {
        m_sprite.update(dt);
        return;
    }

    // 2) Si estamos en salto (aunque debería haber sido detectado en empezar de actualizar),
    //    solo avanzamos el frame de salto
    if (m_isJumping) {
        m_sprite.update(dt);
        return;
    }

    // 3) Si velocidad horizontal es casi cero → Idle
    if (qFuzzyCompare(vx, 0.0f)) {
        if (m_facingLeft) {
            m_sprite.setState(SpriteState::IdleLeft);
        } else {
            m_sprite.setState(SpriteState::Idle);
        }
    }
    // 4) Movimiento a la izquierda
    else if (vx < 0.0f) {
        m_facingLeft = true;
        const float runThreshold = 160.0f;
        if (qAbs(vx) > runThreshold) {
            m_sprite.setState(SpriteState::RunningLeft);
        } else {
            m_sprite.setState(SpriteState::WalkingLeft);
        }
    }
    // 5) Movimiento a la derecha
    else { // vx > 0
        m_facingLeft = false;
        const float runThreshold = 160.0f;
        if (vx > runThreshold) {
            m_sprite.setState(SpriteState::Running);
        } else {
            m_sprite.setState(SpriteState::Walking);
        }
    }

    // 6) Avanzamos el frame de la animación elegida
    m_sprite.update(dt);
}
