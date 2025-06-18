#pragma once
#include "enemigo.h"

class Minotaur : public Enemigo
{
    Q_OBJECT
public:
    explicit Minotaur(QObject* parent = nullptr);

    /* Enemigo overrides */
    void update(float dt) override;
    void takeDamage(int dmg) override;
    QRectF       boundingRect() const override;
    QPainterPath shape()        const override;

private:
    enum class Mode { None, Chase, Patrol, Attack };

    void updateAI(float dt);
    void startDeath();

    /* IA */
    Mode  m_mode       = Mode::None;
    float m_patrolTime = 0.f;
    int   m_patrolDir  = +1;
    bool  m_facingRight= true;

    /* muerte */
    bool  m_deathStarted = false;
    float m_deathTimer   = 0.f;
};
