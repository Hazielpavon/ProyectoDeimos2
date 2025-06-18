// cannon.cpp
#include "cannon.h"
#include "bullet.h"
#include "jugador.h"
#include <QGraphicsPixmapItem>
#include <QtMath>
#include <qgraphicsscene.h>

Cannon::Cannon(Jugador*        player,
               QGraphicsScene* scene,
               qreal           spawnX,
               Position        pos,
               QObject*        parent)
    : QObject(parent)
    , m_player(player)
    , m_scene(scene)
{
    QPixmap tex(":/resources/Cannon.png");
    tex = tex.scaled(tex.width()/2, tex.height()/2,
                     Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_sprite = m_scene->addPixmap(tex);
    m_sprite->setTransformOriginPoint(tex.width()/2, tex.height()/2);

    // X = spawnX, Y arriba o justo encima del suelo
    qreal y = (pos==Top)
                  ? 0.0
                  : scene->sceneRect().height() - tex.height();
    m_sprite->setPos(spawnX - tex.width()/2, y);
}

void Cannon::update(float dt)
{
    if (!m_player) return;
    m_cooldown = std::max(0.0f, m_cooldown - dt);

    QPointF  c    = m_sprite->sceneBoundingRect().center();
    QPointF  p    = m_player->graphicsItem()->sceneBoundingRect().center();
    QVector2D d(p.x()-c.x(), p.y()-c.y());
    float     dist = d.length();
    if (dist > m_range) return;

    // gira el cañón para que apunte al jugador
    float ang = qRadiansToDegrees(std::atan2(d.y(), d.x()));
    m_sprite->setRotation(ang);

    // dispara
    if (m_cooldown <= 0.0f) {
        QVector2D dir = d.normalized();
        new Bullet(c, dir * m_bulletSpeed, m_player, m_scene);
        m_cooldown = m_fireRate;
    }
}
