#pragma once
#include "Enemigo.h"

class BringerOfDeath : public Enemigo
{
    Q_OBJECT
public:
    explicit BringerOfDeath(QObject* parent=nullptr);

    /* IA + animación + física */
    void update(float dt) override;

private:
    void updateAI(float dt);

    float m_patrolDir   = 1.0f;      // izquierda -1 / derecha 1
    float m_patrolTime  = 0.0f;      // tiempo acumulado
};
