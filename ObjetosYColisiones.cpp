#include "ObjetosYColisiones.h"
#include "entidad.h"

ObjetosYColisiones::ObjetosYColisiones(QGraphicsScene* scene, QObject* parent)
    : QObject(parent),
    m_scene(scene)
{}

QGraphicsRectItem* ObjetosYColisiones::addRect(const QRectF& area,
                                               const QColor& color,
                                               bool collisionOnly)
{
    auto* hit = new QGraphicsRectItem(area);
    hit->setPen(Qt::NoPen);
    hit->setBrush(Qt::NoBrush);
    hit->setZValue(1.0);
    m_scene->addItem(hit);

    QGraphicsRectItem* vis = nullptr;
    if (!collisionOnly) {
        vis = new QGraphicsRectItem(area);
        vis->setBrush(color);
        vis->setPen(Qt::NoPen);
        vis->setZValue(0.5);
        m_scene->addItem(vis);
    }

    m_objetos.push_back({ vis, hit });
    return hit;
}

void ObjetosYColisiones::resolveCollisions(entidad* player, float /*dt*/)
{
    if (!player) return;

    // 1) reset suelo
    player->setOnGround(false);

    // 2) datos de sprite
    QPointF footPos = player->transform().getPosition();
    QSize   sprSize = player->sprite().getSize();
    float   halfW   = sprSize.width()  / 2.0f;
    float   fullH   = sprSize.height();

    float vx = player->fisica().velocity().x();
    float vy = player->fisica().velocity().y();

    QRectF rectJugador(footPos.x() - halfW,
                       footPos.y() - fullH,
                       sprSize.width(),
                       sprSize.height());

    // 3) colisiones
    for (const auto& obj : m_objetos) {
        QRectF rectObj = obj.hitbox->sceneBoundingRect();
        if (!rectJugador.intersects(rectObj))
            continue;

        float pieY    = footPos.y();
        float cabezaY = footPos.y() - fullH;

        // aterrizar
        if (vy >= 0.0f && pieY >= rectObj.top() && cabezaY < rectObj.top()) {
            footPos.setY(rectObj.top());
            player->fisica().setVelocity(vx, 0.0f);
            player->setOnGround(true);
        }
        // choque cabeza
        else if (vy < 0.0f && cabezaY <= rectObj.bottom() && pieY > rectObj.bottom()) {
            footPos.setY(rectObj.bottom() + fullH);
            player->fisica().setVelocity(vx, 0.0f);
        }

        rectJugador.moveTopLeft(QPointF(footPos.x() - halfW,
                                        footPos.y() - fullH));
    }

    // 4) aplica piePos
    player->transform().setPosition(footPos.x(), footPos.y());
}

