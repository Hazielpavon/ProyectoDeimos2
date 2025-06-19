//minotaur.h

#pragma once
#include "Enemigo.h"

/* ════════════════════════════════════════════════════════ *
 *  Minotauro — jefe sin animación de muerte                *
 *  (al morir simplemente desaparece)                       *
 * ════════════════════════════════════════════════════════ */
class Minotaur : public Enemigo
{
    Q_OBJECT
public:
    explicit Minotaur(QObject* parent = nullptr);

    /* Enemigo overrides */
    void update(float dt) override;
    void takeDamage(int dmg) override;

    /* QGraphicsItem overrides (hitbox rectangular) */
    QRectF extracted(const QPixmap &p) const;
    QRectF boundingRect() const override;
    QPainterPath shape()        const override;

private:
    /* IA interna */
    enum class Mode { None, Chase, Patrol, Attack };
    void updateAI(float dt);

    Mode  m_mode        = Mode::None;
    float m_patrolTime  = 0.f;
    int   m_patrolDir   = +1;
    bool  m_facingRight = true;

    /* flag de muerte (sin anim) */
    bool  m_dead        = false;
};
