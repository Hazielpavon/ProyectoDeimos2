#include "ObjetosYColisiones.h"
#include "entidad.h"
#include <QPointF>
#include <QDebug>

static constexpr float HITBOX_W_FACTOR = 0.40f;
static constexpr float HITBOX_H_FACTOR = 0.71f;

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

    // visual opcional
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

void ObjetosYColisiones::resolveCollisions(entidad* player,
                                           const QSize& pixSize,
                                           float /*dt*/)
{
    if (!player) return;

    // 1) reset suelo
    player->setOnGround(false);

    // 2) datos de posición y velocidad
    QPointF footPos = player->transform().getPosition();
    float vx = player->fisica().velocity().x();
    float vy = player->fisica().velocity().y();

    // 3) calculamos un hitbox proporcional
    float hitW   = pixSize.width()  * HITBOX_W_FACTOR;
    float hitH   = pixSize.height() * HITBOX_H_FACTOR;
    float halfW  = hitW  * 0.5f;
    float fullH  = hitH;  // diámetro vertical del hitbox

    QRectF rectJugador(
        footPos.x() - halfW,
        footPos.y() - fullH,
        hitW,
        hitH);

    // 4) colisiones con cada objeto
    for (const auto& obj : m_objetos) {
        QRectF rectObj = obj.hitbox->sceneBoundingRect();
        if (!rectJugador.intersects(rectObj))
            continue;

        float pieY    = footPos.y();
        float cabezaY = footPos.y() - fullH;

        // aterrizar
        if (vy >= 0.0f
            && pieY    >= rectObj.top()
            && cabezaY <  rectObj.top())
        {
            footPos.setY(rectObj.top());
            player->fisica().setVelocity(vx, 0.0f);
            player->setOnGround(true);
        }
        // choque de cabeza
        else if (vy < 0.0f
                 && cabezaY <= rectObj.bottom()
                 && pieY    >  rectObj.bottom())
        {
            footPos.setY(rectObj.bottom() + fullH);
            player->fisica().setVelocity(vx, 0.0f);
        }

        // actualizar rectJugador tras mover footPos
        rectJugador.moveTopLeft(
            QPointF(footPos.x() - halfW,
                    footPos.y() - fullH));
    }

    // 5) aplicar nueva posición
    player->transform().setPosition(footPos.x(), footPos.y());
}
