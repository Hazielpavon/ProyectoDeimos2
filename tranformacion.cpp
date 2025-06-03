#include "tranformacion.h"

tranformacion::tranformacion()
    : m_pos(0, 0)
{
}

void tranformacion::setPosition(int x, int y)
{
    m_pos.setX(x);
    m_pos.setY(y);
}

QPoint tranformacion::getPosition() const
{
    return m_pos;
}
