#pragma once
#include "enemigo.h"

class Skeleton : public Enemigo
{
    Q_OBJECT
public:
    explicit Skeleton(QObject* parent = nullptr);

    /* overrides */
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
    float m_patrolTime = 0.0f;
    int   m_patrolDir  = +1;
    bool  m_faceRight  = true;

    /* muerte */
    bool  m_dying     = false;
    float m_dieTimer  = 0.0f;

    /* línea de pies detectada */
    int   m_baseline  = 0;          //  👈  NUEVO
};
