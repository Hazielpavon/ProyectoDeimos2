#pragma once
#include "Enemigo.h"

class Carnivore : public Enemigo
{
    Q_OBJECT
public:
    explicit Carnivore(QObject* parent = nullptr);

    /* ciclo de vida */
    void update(float dt) override;
    void takeDamage(int dmg) override;

    /* colisiones Qt */
    QRectF       boundingRect() const override;
    QPainterPath shape()        const override;

private:
    enum class Mode { None, Chase, Patrol, Attack, Jump };
    void updateAI(float dt);
    void startJump();
    void startDeath();

    Mode  m_mode        = Mode::None;
    float m_patrolTime  = 0.f;
    int   m_patrolDir   = +1;
    bool  m_faceRight   = true;

    /* salto */
    bool  m_jumping   = false;
    float m_jumpCD    = 0.f;

    /* muerte */
    bool  m_dying     = false;
    float m_dieTimer  = 0.f;
};
