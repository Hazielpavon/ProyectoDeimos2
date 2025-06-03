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
    // Cargamos las animaciones
    m_sprite.loadFrames(SpriteState::Walking,":/resources/0_Blood_Demon_Walking_",24);
    m_sprite.loadFrames(SpriteState::Idle,":/resources/0_Blood_Demon_Idle_",16);
    m_sprite.loadFrames(SpriteState::IdleLeft, ":/resources/0_Blood_Demon_IdleL_",16);
    m_sprite.loadFrames(SpriteState::WalkingLeft,":/resources/0_Blood_Demon_WalkingL_",24);
    m_sprite.loadFrames(SpriteState::Jump, ":/resources/0_Blood_Demon_Jump Loop_",6);
    m_sprite.generateMirroredFrames(SpriteState::Jump,SpriteState::JumpLeft);
    m_sprite.loadFrames(SpriteState::Running, ":/resources/0_Blood_Demon_Running_",12);
    m_sprite.generateMirroredFrames(SpriteState::Running,SpriteState::RunningLeft);

    // Configuraciones generales de fps y size:
    m_sprite.setFPS(12);
    m_sprite.setSize(128, 128);

    // No fijamos aquí m_groundY, lo haremos en startJump()
    m_sprite.setState(SpriteState::Idle);
    m_componenteSalud.setHP(100);
}

entidad::~entidad(){

}

void entidad::startJump()
{
    if (m_isJumping) return; // ya está en el aire → ignoramos doble salto

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
    if (m_isJumping) {
        actualizarSalto(dt);
    }

    m_componenteFisico.actualizar(dt);

    QPointF posF = m_transformacion.getPosition();
    int spriteW = m_sprite.getSize().width();
    int spriteH = m_sprite.getSize().height();
    int drawX = int(posF.x() - (spriteW * 0.5f));
    int drawY = int(posF.y() - (spriteH * 0.5f));
    m_sprite.setPosition(drawX, drawY);

    if (!m_isJumping) {
        actualizarAnimacion(dt);
    } else {
        m_sprite.update(dt);
    }

    m_componenteSalud.actualizar(dt);
}

void entidad::actualizarSalto(float dt)
{

    QPointF pos = m_transformacion.getPosition();
    pos.setY(pos.y() + m_verticalVelocity * dt);

    m_verticalVelocity += GRAVITY * dt;


    if (pos.y() >= m_groundY) {
        pos.setY(m_groundY);
        m_verticalVelocity = 0.0f;
        m_isJumping = false;

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

    const float runThreshold = 160.0f;

    if (qFuzzyCompare(vx, 0.0f)) {
        if (m_facingLeft) {
            m_sprite.setState(SpriteState::IdleLeft);
        } else {
            m_sprite.setState(SpriteState::Idle);
        }
    }
    else if (vx < 0.0f) {
        m_facingLeft = true;

        if (qAbs(vx) > runThreshold) {
            m_sprite.setState(SpriteState::RunningLeft);
        } else {
            m_sprite.setState(SpriteState::WalkingLeft);
        }
    }
    else {
        m_facingLeft = false;

        if (vx > runThreshold) {
            m_sprite.setState(SpriteState::Running);
        } else {
            m_sprite.setState(SpriteState::Walking);
        }
    }
    m_sprite.update(dt);
}

