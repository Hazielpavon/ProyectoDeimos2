// tranformacion.cpp
#include "tranformacion.h"

tranformacion::tranformacion()
    : m_pos(0.0f, 0.0f)
{
}

void tranformacion::setPosition(qreal x, qreal y)
{
    m_pos.setX(x);
    m_pos.setY(y);
}

QPointF tranformacion::getPosition() const
{
    return m_pos;
}
