// cannon.h
#pragma once
#include <QObject>
class Jugador;
class QGraphicsScene;
class QGraphicsPixmapItem;

class Cannon : public QObject {
    Q_OBJECT
public:
    enum Position { Top, Bottom };

    // ahora recibe un X (spawnX) y la posición (arriba/abajo)
    Cannon(Jugador*        player,
           QGraphicsScene* scene,
           qreal           spawnX,
           Position        pos,
           QObject*        parent=nullptr);

    void update(float dt);
    void setFireRate(float s) { m_fireRate = s; }
    float fireRate() const { return m_fireRate; }
private:
    Jugador*            m_player;
    QGraphicsScene*     m_scene;
    QGraphicsPixmapItem* m_sprite;
    float               m_cooldown    = 0.0f;
    float         m_fireRate    = 2.0f;
    const float         m_range       = 500.0f;
    const float         m_bulletSpeed = 600.0f;
};
