#pragma once
#include "Enemigo.h"

class BringerOfDeath : public Enemigo {
    Q_OBJECT
public:
    explicit BringerOfDeath(QObject* parent = nullptr);
    void takeDamage(int dmg) override;
    void update(float dt) override;
    void updateAI(float dt);
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    bool m_facingRight = true;
    enum class Mode { None, Chase, Patrol, Attack };
    Mode      m_mode        = Mode::None;
      QTransform m_flipTransform;
    // IA internal
    float m_patrolTime = 0.0f;
    int   m_patrolDir  = +1;   // +1 = derecha, -1 = izquierda
    int   m_lastDir    = +1;   // última dirección al jugador
private slots:
    void onDeathAnimFinished();

private:
    bool m_deathStarted = false;
    bool m_deathFinished = false;
    float m_deathTimer   = 0.0f;
};
