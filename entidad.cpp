// entity.cpp
#include "entidad.h"
#include <QtMath>

/* ===========================================================
 *  Constructor: carga sprites por defecto
 * =========================================================== */
entidad::entidad()
    : m_transformacion()
    , m_sprite()
    , m_componenteFisico(&m_transformacion)
{
    /* sprites Demon ------------------------------------------------ */
    m_sprite.loadFrames(SpriteState::Walking ,":/resources/0_Blood_Demon_Walking_" ,24);
    m_sprite.loadFrames(SpriteState::Idle    ,":/resources/0_Blood_Demon_Idle_"    ,16);
    m_sprite.loadFrames(SpriteState::IdleLeft,":/resources/0_Blood_Demon_IdleL_"   ,16);
    m_sprite.loadFrames(SpriteState::WalkingLeft,":/resources/0_Blood_Demon_WalkingL_",24);
    m_sprite.loadFrames(SpriteState::Jump   ,":/resources/0_Blood_Demon_Jump Loop_",6);
    m_sprite.generateMirroredFrames(SpriteState::Jump, SpriteState::JumpLeft);
    m_sprite.loadFrames(SpriteState::Running,":/resources/0_Blood_Demon_Running_" ,12);
    m_sprite.generateMirroredFrames(SpriteState::Running, SpriteState::RunningLeft);
    m_sprite.loadFrames(SpriteState::Slashing ,":/resources/0_Blood_Demon_Slashing_",12);
    m_sprite.generateMirroredFrames(SpriteState::Slashing, SpriteState::SlashingLeft);
    m_sprite.loadFrames(SpriteState::Slidding ,":/resources/0_Blood_Demon_Sliding_",6);
    m_sprite.generateMirroredFrames(SpriteState::Slidding, SpriteState::SliddingLeft);
    m_sprite.loadFrames(SpriteState::dead     ,":/resources/0_Blood_Demon_Dying_",15);
    m_sprite.generateMirroredFrames(SpriteState::dead, SpriteState::deadleft);
    m_sprite.loadFrames(SpriteState::throwing ,":/resources/0_Blood_Demon_Throwing_",12);
    m_sprite.generateMirroredFrames(SpriteState::throwing, SpriteState::throwingLeft);

    m_sprite.setFPS(12);
    m_sprite.setSize(128,128);
    m_sprite.setState(SpriteState::Idle);

    m_componenteSalud.setHP(100);
}

/* ---------------- setters simples ---------------- */
void entidad::setOnGround(bool g){ m_onGround = g; }
bool entidad::isOnGround() const { return m_onGround; }
void entidad::setLastDirection(SpriteState d){ m_lastDirection = d; }
SpriteState entidad::getLastDirection() const{ return m_lastDirection; }

/* ===========================================================
 *  Salto (multiplicado si se ha desbloqueado la habilidad)
 * =========================================================== */
void entidad::startJump()
{
    if (!m_onGround) return;

    const float vy = -JUMP_VELOCITY * m_jumpMultiplier;
    m_componenteFisico.setVelocity(m_componenteFisico.velocity().x(), vy);
    m_onGround = false;
    m_sprite.setState(m_facingLeft ? SpriteState::JumpLeft : SpriteState::Jump);
}

/* Animación temporal ------------------------------------------------ */
void entidad::reproducirAnimacionTemporal(SpriteState st, float seg)
{
    m_animTemporal = st;
    m_animTempTime = seg;
    m_sprite.setState(st);
}

/* ===========================================================
 *  Bucle de actualización
 * =========================================================== */
void entidad::actualizar(float dt)
{
    /* 1) animaciones */
    if (m_animTempTime > 0.f) {
        m_animTempTime -= dt;
        m_sprite.update(dt);
    } else {
        if (m_onGround) actualizarAnimacion(dt);
        else            m_sprite.update(dt);
    }

    /* 2) gravedad */
    if (!m_onGround) {
        float vy = m_componenteFisico.velocity().y() + GRAVITY*dt;
        m_componenteFisico.setVelocity(m_componenteFisico.velocity().x(), vy);
    }

    /* 3) integrar posición */
    QPointF pos = m_transformacion.getPosition();
    pos.rx() += m_componenteFisico.velocity().x() * dt;
    pos.ry() += m_componenteFisico.velocity().y() * dt;
    m_transformacion.setPosition(pos.x(), pos.y());

    /* 4) componentes */
    m_componenteFisico.actualizar(dt);
    m_componenteSalud.actualizar(dt);

    /* 5) el nivel, al colisionar, volverá a poner m_onGround=true */
    m_onGround = false;
}

/* Animación en suelo ---------------------------------------- */
void entidad::actualizarAnimacion(float dt)
{
    const float vx = m_componenteFisico.velocity().x();
    const float RUN_THR = 160.f;

    if (qFuzzyIsNull(vx)) {
        m_sprite.setState(m_facingLeft ? SpriteState::IdleLeft
                                       : SpriteState::Idle);
    } else if (vx < 0.f) {
        m_facingLeft    = true;
        m_lastDirection = SpriteState::WalkingLeft;
        m_sprite.setState(qAbs(vx) > RUN_THR ? SpriteState::RunningLeft
                                             : SpriteState::WalkingLeft);
    } else {
        m_facingLeft    = false;
        m_lastDirection = SpriteState::Walking;
        m_sprite.setState(vx > RUN_THR ? SpriteState::Running
                                       : SpriteState::Walking);
    }
    m_sprite.update(dt);
}
