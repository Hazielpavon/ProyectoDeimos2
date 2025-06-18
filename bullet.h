// bullet.h
#pragma once
#include <QObject>
#include <QGraphicsPixmapItem>
#include <QVector2D>
class Jugador;
class QGraphicsScene;

class Bullet : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    Bullet(const QPointF&   start,
           const QVector2D& velocity,
           Jugador*         target,
           QGraphicsScene*  scene,
           QObject*         parent = nullptr);

    // Qt llamará advance(0) y luego advance(1) cada frame
    void advance(int phase) override;

private:
    QVector2D m_velocity;
    Jugador*  m_target;
};
