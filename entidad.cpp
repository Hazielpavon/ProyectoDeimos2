#include "entidad.h"
#include <QDebug>

entidad::entidad()
    : m_transformacion(),
    m_sprite(),
    m_componenteFisico(&m_transformacion),
    m_componenteSalud(),
    m_onGround(false),
    m_lastDirection(SpriteState::Idle),
    m_facingLeft(false)
{
    m_sprite.loadFrames(SpriteState::Walking, ":/resources/0_Blood_Demon_Walking_", 24);
    m_sprite.loadFrames(SpriteState::Idle, ":/resources/0_Blood_Demon_Idle_", 16);
    m_sprite.loadFrames(SpriteState::IdleLeft, ":/resources/0_Blood_Demon_IdleL_", 16);
    m_sprite.loadFrames(SpriteState::WalkingLeft, ":/resources/0_Blood_Demon_WalkingL_", 24);
    m_sprite.loadFrames(SpriteState::Jump, ":/resources/0_Blood_Demon_Jump Loop_", 6);
    m_sprite.generateMirroredFrames(SpriteState::Jump, SpriteState::JumpLeft);
    m_sprite.loadFrames(SpriteState::Running, ":/resources/0_Blood_Demon_Running_", 12);
    m_sprite.generateMirroredFrames(SpriteState::Running, SpriteState::RunningLeft);

    m_sprite.setFPS(12);
    m_sprite.setSize(128, 128);
    m_sprite.setState(SpriteState::Idle);
    m_componenteSalud.setHP(100);
}

entidad::~entidad() {}

void entidad::setOnGround(bool enSuelo) {
    m_onGround = enSuelo;
}

bool entidad::isOnGround() const {
    return m_onGround;
}

void entidad::setLastDirection(SpriteState dir) {
    m_lastDirection = dir;
}

SpriteState entidad::getLastDirection() const {
    return m_lastDirection;
}

void entidad::startJump() {
    if (!m_onGround) return;

    qDebug() << "[entidad] ¡Salto iniciado!";
    m_componenteFisico.setVelocity(m_componenteFisico.velocity().x(), -550.0f);
    m_onGround = false;

    m_sprite.setState(m_facingLeft ? SpriteState::JumpLeft : SpriteState::Jump);
}

void entidad::actualizar(float dt) {
    if (!m_onGround) {
        float nuevaVy = m_componenteFisico.velocity().y() + 980.0f * dt;
        m_componenteFisico.setVelocity(m_componenteFisico.velocity().x(), nuevaVy);
    }

    float nuevaY = m_transformacion.getPosition().y() + m_componenteFisico.velocity().y() * dt;
    m_transformacion.setPosition(m_transformacion.getPosition().x(), nuevaY);

    m_componenteFisico.actualizar(dt);

    QPointF pos = m_transformacion.getPosition();
    QSize spriteSize = m_sprite.getSize();
    int drawX = int(pos.x() - spriteSize.width() * 0.5f);
    int drawY = int(pos.y() - spriteSize.height() * 0.5f);
    m_sprite.setPosition(drawX, drawY);

    if (m_onGround) {
        actualizarAnimacion(dt);
    } else {
        m_sprite.update(dt);
    }

    m_componenteSalud.actualizar(dt);
    m_onGround = false;
}

void entidad::actualizarAnimacion(float dt) {
    float vx = m_componenteFisico.velocity().x();
    const float runThreshold = 160.0f;

    if (qFuzzyCompare(vx, 0.0f)) {
        if (m_lastDirection == SpriteState::WalkingLeft || m_facingLeft)
            m_sprite.setState(SpriteState::IdleLeft);
        else
            m_sprite.setState(SpriteState::Idle);
    } else if (vx < 0.0f) {
        m_facingLeft = true;
        m_lastDirection = SpriteState::WalkingLeft;
        m_sprite.setState(qAbs(vx) > runThreshold ? SpriteState::RunningLeft : SpriteState::WalkingLeft);
    } else {
        m_facingLeft = false;
        m_lastDirection = SpriteState::Walking;
        m_sprite.setState(vx > runThreshold ? SpriteState::Running : SpriteState::Walking);
    }

    m_sprite.update(dt);
}
