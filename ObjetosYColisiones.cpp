#include "ObjetosYColisiones.h"
#include "entidad.h"
#include "enemigo.h"
#include <QPointF>
#include <QGraphicsScene>
ObjetosYColisiones::ObjetosYColisiones(QGraphicsScene* scene, QObject* parent)
    : QObject(parent)
    , m_scene(scene)
{}

QGraphicsRectItem* ObjetosYColisiones::addRect(const QRectF& area,
                                               const QColor& color,
                                               bool collisionOnly)
{
    // hitbox (invisible)
    auto* hit = new QGraphicsRectItem(area);
    hit->setPen(Qt::NoPen);
    hit->setBrush(Qt::NoBrush);
    hit->setZValue(1.0);
    m_scene->addItem(hit);

    // opcional visual
    QGraphicsRectItem* vis = nullptr;
    if (!collisionOnly) {
        vis = new QGraphicsRectItem(area);
        vis->setPen(Qt::NoPen);
        vis->setBrush(color);
        vis->setZValue(0.5);
        m_scene->addItem(vis);
    }

    m_objetos.push_back({vis, hit});
    return hit;
}

void ObjetosYColisiones::resolveCollisions(entidad* player,
                                           const QSize& pixSize,
                                           float /*dt*/)
{
    if (!player) return;
    player->setOnGround(false);

    QPointF footPos = player->transform().getPosition();
    float vx = player->fisica().velocity().x();
    float vy = player->fisica().velocity().y();

    float hitW  = pixSize.width()  * HITBOX_W_FACTOR_P;
    float hitH  = pixSize.height() * HITBOX_H_FACTOR_P;
    float halfW = hitW * 0.5f;

    QRectF rectJ(footPos.x() - halfW,
                 footPos.y() - hitH,
                 hitW, hitH);

    for (auto& obj : m_objetos) {
        QRectF rObj = obj.hitbox->sceneBoundingRect();
        if (!rectJ.intersects(rObj)) continue;

        float pieY    = footPos.y();
        float cabezaY = pieY - hitH;

        // aterrizar
        if (vy >= 0 && pieY >= rObj.top() && cabezaY < rObj.top()) {
            footPos.setY(rObj.top());
            player->fisica().setVelocity(vx, 0);
            player->setOnGround(true);
        }
        // choque de cabeza
        else if (vy < 0 && cabezaY <= rObj.bottom() && pieY > rObj.bottom()) {
            footPos.setY(rObj.bottom() + hitH);
            player->fisica().setVelocity(vx, 0);
        }

        rectJ.moveTopLeft({footPos.x() - halfW, footPos.y() - hitH});
    }

    player->transform().setPosition(footPos.x(), footPos.y());
}

void ObjetosYColisiones::resolveCollisions(Enemigo* e,
                                           const QSize& pixSize,
                                           float /*dt*/)
{
    if (!e) return;

    // calculamos pies en escena
    QPointF center = e->pos();
    float halfH = pixSize.height() * 0.5f;
    QPointF footPos(center.x(), center.y() + halfH);

    float vx = e->velX();
    float vy = e->velY();

    float hitW  = pixSize.width()  * HITBOX_W_FACTOR_E;
    float hitH  = pixSize.height() * HITBOX_H_FACTOR_E;
    float halfW = hitW * 0.5f;

    QRectF rectE(footPos.x() - halfW,
                 footPos.y() - hitH,
                 hitW, hitH);

    for (auto& obj : m_objetos) {
        QRectF rObj = obj.hitbox->sceneBoundingRect();
        if (!rectE.intersects(rObj)) continue;

        float pieY    = footPos.y();
        float cabezaY = pieY - hitH;

        // aterrizar
        if (vy >= 0 && pieY >= rObj.top() && cabezaY < rObj.top()) {
            footPos.setY(rObj.top());
            e->setVelY(0);
        }
        // choque de cabeza
        else if (vy < 0 && cabezaY <= rObj.bottom() && pieY > rObj.bottom()) {
            footPos.setY(rObj.bottom() + hitH);
            e->setVelY(0);
        }

        rectE.moveTopLeft({footPos.x() - halfW, footPos.y() - hitH});
    }

    // reubicamos al centro
    e->setPos(footPos.x(), footPos.y() - halfH);
}
