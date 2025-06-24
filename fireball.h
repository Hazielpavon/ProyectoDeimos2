#pragma once

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPixmap>
#include <QVector>
#include <QPointF>
#include <QString>
#include "enemigo.h"
class Enemigo;
class QGraphicsScene;

class Fireball : public QObject, public QGraphicsPixmapItem {
public:
    Fireball(bool haciaIzquierda,
             QPointF origen,
             QGraphicsScene* scene,
             QVector<Enemigo*>* enemigos);

    void avanzar(float dt);
    bool isAlive() const { return m_alive; }

private:
    QVector<QPixmap> frames;
    int frameIndex = 0;
    float tiempoAcumulado = 0;
    float velX;
    QGraphicsScene* m_scene;
    QVector<Enemigo*>* m_enemigos;
    bool m_alive = true;
};
