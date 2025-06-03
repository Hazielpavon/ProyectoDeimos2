#ifndef TRANFORMACION_H
#define TRANFORMACION_H

#include <QPoint>

/**
 * @brief Transformacion guarda únicamente una posición (x,y).
 */
class tranformacion
{
public:
    tranformacion();

    // Fija la posición interna.
    void setPosition(int x, int y);

    // Devuelve la posición interna (QPoint).
    QPoint getPosition() const;

private:
    QPoint m_pos;
};

#endif // TRANSFORMACION_H
