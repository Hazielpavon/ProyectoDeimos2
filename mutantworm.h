#pragma once
#include "enemigo.h"

class MutantWorm : public Enemigo
{
    Q_OBJECT
public:
    explicit MutantWorm(QObject* parent = nullptr);

    void setStationary(bool st) { m_stationary = st; }

    void        update(float dt) override;
    void        takeDamage(int dmg) override;
    QRectF      boundingRect() const override;
    QPainterPath shape()        const override;

protected:
    void onRevive() override
    {
        Enemigo::onRevive();
        m_deadAnim   = false;
        m_patrolTime = 0.f;
        m_mode       = Mode::Patrol;
    }

private:
    enum class Mode { None, Patrol, Chase, Attack, Jump };

    void  updateAI(float dt);
    void  startJump();
    void  startDeath();

    Mode  m_mode        = Mode::None;
    float m_patrolTime  = 0.f;
    int   m_patrolDir   = +1;
    bool  m_faceRight   = true;

    bool  m_jumping      = false;
    float m_jumpCooldown = 0.f;

    bool  m_deadAnim     = false;
    float m_deadTimer    = 0.f;

    bool  m_stationary   = false;
};
