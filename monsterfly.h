#pragma once
#include "Enemigo.h"

/*  Enemigo volador (Monster 0 – mosca)  */
class MonsterFly : public Enemigo
{
    Q_OBJECT
public:
    explicit MonsterFly(QObject* parent = nullptr);

    /* Enemigo overrides */
    void update(float dt) override;
    void takeDamage(int dmg) override;
    QRectF       boundingRect() const override;
    QPainterPath shape()        const override;

private:
    enum class Mode { None, Chase, Patrol, Attack };

    /* IA */
    void updateAI(float dt);
    void startDeath();

    Mode  m_mode        = Mode::None;
    float m_patrolTime  = 0.f;
    int   m_patrolDir   = +1;
    bool  m_faceRight   = true;

    /* bobbing vertical */
    float m_bobTime     = 0.f;
    float m_prevBob     = 0.f;     // <<——  NUEVO

    /* muerte */
    bool  m_deadAnim    = false;
    float m_deadTimer   = 0.f;
};
