#pragma once
#include "enemigo.h"

class Demon : public Enemigo
{
    Q_OBJECT
public:
    explicit Demon(QObject* parent = nullptr);

    void update(float dt) override;
    void takeDamage(int dmg) override;

    QRectF        boundingRect() const override;
    QPainterPath  shape()        const override;
protected:
    void onRevive() override {
        Enemigo::onRevive();
        m_deathStarted = false;
        m_patrolTime = 0;
        m_mode = Mode::Patrol;
    }
private:
    enum class Mode { None, Chase, Patrol, Attack };

    void updateAI(float dt);
    void startDeath();


    Mode   m_mode        = Mode::None;
    float  m_patrolTime  = 0.0f;
    int    m_patrolDir   = +1;
    bool   m_facingRight = true;
    bool  m_dead;

    bool   m_deathStarted  = false;
    float  m_deathTimer    = 0.0f;
};
