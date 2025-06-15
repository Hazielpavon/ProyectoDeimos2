#pragma once
#include "Enemigo.h"

class Demon : public Enemigo
{
    Q_OBJECT
public:
    explicit Demon(QObject* parent = nullptr);

    /* Enemigo overrides */
    void update(float dt) override;
    void takeDamage(int dmg) override;

    /* Bounding rect so QGraphicsScene can collide */
    QRectF        boundingRect() const override;
    QPainterPath  shape()        const override;

private:
    enum class Mode { None, Chase, Patrol, Attack };

    void updateAI(float dt);
    void startDeath();

    /* IA helpers */
    Mode   m_mode        = Mode::None;
    float  m_patrolTime  = 0.0f;
    int    m_patrolDir   = +1;
    bool   m_facingRight = true;

    /* Death handling */
    bool   m_deathStarted  = false;
    float  m_deathTimer    = 0.0f;
};
