#pragma once
#include "enemigo.h"

/* ══════════════════════════════════════════════════════ */
/*  Mutant Worm – Monster 1                               */
/*  Animaciones: Idle · Walk · Attack · Jump · Dying      */
/* ══════════════════════════════════════════════════════ */
class MutantWorm : public Enemigo
{
    Q_OBJECT
public:
    explicit MutantWorm(QObject* parent = nullptr);

    /* Enemigo overrides */
    void update(float dt) override;
    void takeDamage(int dmg) override;
    QRectF       boundingRect() const override;
    QPainterPath shape()        const override;
protected:
    void onRevive() override {
        // llamamos al padre ya hecho, luego reseteamos lo propio
        Enemigo::onRevive();
        m_deadAnim = false;      // vuelve a procesar update()
        m_patrolTime = 0;    // opcional, reinicio de patrulla
        m_mode = Mode::Patrol;
    }
private:
    enum class Mode { None, Chase, Patrol, Attack, Jump };

    void updateAI(float dt);
    void startJump();
    void startDeath();

    /* IA */
    Mode  m_mode        = Mode::None;
    float m_patrolTime  = 0.f;
    int   m_patrolDir   = +1;
    bool  m_faceRight   = true;

    /* jump */
    bool  m_jumping     = false;
    float m_jumpCooldown= 0.f;

    /* death */
    bool  m_deadAnim    = false;
    float m_deadTimer   = 0.f;
};
