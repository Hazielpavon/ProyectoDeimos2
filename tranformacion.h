// tranformacion.h
#ifndef TRANFORMACION_H
#define TRANFORMACION_H

#include <QPointF>

class tranformacion {
public:
    tranformacion();
    void setPosition(qreal x, qreal y);
    QPointF getPosition() const;

private:
    QPointF m_pos;
};

#endif // TRANFORMACION_H
