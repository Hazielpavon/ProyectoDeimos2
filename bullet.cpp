// bullet.cpp
#include "bullet.h"
#include "jugador.h"
#include <QGraphicsScene>
#include <QtMath>

Bullet::Bullet(const QPointF&   start,
               const QVector2D& velocity,
               Jugador*         target,
               QGraphicsScene*  scene,
               QObject*         parent)
    : QObject(parent)
    , QGraphicsPixmapItem(
          QPixmap(":/resources/Bullet_Cannon.png")
              .scaled(32,32, Qt::KeepAspectRatio, Qt::SmoothTransformation))
    , m_velocity(velocity)
    , m_target(target)
{
    setOffset(-pixmap().width()/2, -pixmap().height()/2);
    setPos(start);
    setZValue(2);
    scene->addItem(this);
}

void Bullet::advance(int phase) {
    if (phase == 0) return;
    moveBy(m_velocity.x()/60.0, m_velocity.y()/60.0);
    float ang = qRadiansToDegrees(std::atan2(m_velocity.y(), m_velocity.x()));
    setRotation(ang);

    if (sceneBoundingRect()
            .intersects(m_target->graphicsItem()->sceneBoundingRect()))
    {
        m_target->aplicarDano(10);
        scene()->removeItem(this);
        deleteLater();
        return;
    }

    if (!scene()->sceneRect().contains(pos())) {
        scene()->removeItem(this);
        deleteLater();
    }
}
