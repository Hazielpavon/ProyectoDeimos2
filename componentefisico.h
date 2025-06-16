#ifndef COMPONENTEFISICO_H
#define COMPONENTEFISICO_H

#include "tranformacion.h"
#include <QPointF>

class componentefisico {
public:
    explicit componentefisico(tranformacion *t);
    componentefisico(const componentefisico &o);

    void setSpeed(float pxPerSecond);
    void actualizar(float dt);
    QPointF velocity() const;
    void setDirection(int dir);
    void setVelocity(float vx, float vy);

    /**
     * Establece la aceleración de la gravedad vertical (px/s²).
     * Valores negativos tiran hacia arriba, positivos hacia abajo.
     */
    void setGravity(float g) { m_gravity = g; }

private:
    tranformacion *m_transform;
    QPointF         m_velocity;
    float           m_speed;
    int             m_direction;

    // Aceleración de la gravedad (px/s²). Negativo = hacia arriba, positivo = hacia abajo
    float           m_gravity = -980.0f;
};

#endif // COMPONENTEFISICO_H
