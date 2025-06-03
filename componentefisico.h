#ifndef COMPONENTEFISICO_H
#define COMPONENTEFISICO_H
#include "tranformacion.h"
#include <QPointF>

class componentefisico{

public:
    explicit componentefisico(tranformacion *t);
    componentefisico(const componentefisico &o);
    void setSpeed(float pxPerSecond);
    void actualizar(float dt);
    QPointF velocity() const;
    void setDirection(int dir);
    void setVelocity(float vx, float vy);
private:
    tranformacion *m_transform;
    QPointF         m_velocity;
    float           m_speed;
    int             m_direction;
};


#endif // COMPONENTEFISICO_H
