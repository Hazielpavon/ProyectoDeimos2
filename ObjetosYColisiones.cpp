#include "ObjetosYColisiones.h"
#include "entidad.h"              // tu clase de jugador

ObjetosYColisiones::ObjetosYColisiones(QGraphicsScene* scene, QObject* parent)
    : QObject(parent),
    m_scene(scene)
{}

QGraphicsRectItem* ObjetosYColisiones::addRect(const QRectF& area,
                                               const QColor& color,
                                               bool collisionOnly)
{
    // HitBox: sin relleno / sin borde
    auto* hit = new QGraphicsRectItem(area);
    hit->setPen(Qt::NoPen);
    hit->setBrush(Qt::NoBrush);
    hit->setZValue(1.0);          // debajo del jugador
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
    return hit;                   // por si se quiere guardar
}

/*  ─────────────────────────────────────────
    Resolución de colisiones (solo vertical)
    ───────────────────────────────────────── */
void ObjetosYColisiones::resolveCollisions(entidad* player, float /*dt*/)
{
    if (!player) return;

    // Datos del jugador
    QPointF pos     = player->transform().getPosition();
    QSize   sprSize = player->sprite().getSize();
    float   halfW   = sprSize.width()  / 2.0f;
    float   halfH   = sprSize.height() / 2.0f;

    QRectF rectJugador(pos.x() - halfW,
                       pos.y() - halfH,
                       sprSize.width(),
                       sprSize.height());

    float vx = player->fisica().velocity().x();
    float vy = player->fisica().velocity().y();

    /* Recorremos TODAS las hitboxes */
    for (const auto& obj : m_objetos)
    {
        QRectF rectObj = obj.hitbox->rect();

        if (!rectJugador.intersects(rectObj))
            continue;

        float pie    = pos.y() + halfH;
        float cabeza = pos.y() - halfH;

        // 1) Caída sobre la plataforma
        if (vy >= 0.0f &&
            pie >= rectObj.top() &&
            cabeza <  rectObj.top())
        {
            pos.setY(rectObj.top() - halfH);
            player->fisica().setVelocity(vx, 0.0f);
            player->setOnGround(true);
        }
        // 2) Golpe por abajo (cabeza)
        else if (vy < 0.0f &&
                 cabeza <= rectObj.bottom() &&
                 pie    >  rectObj.bottom())
        {
            pos.setY(rectObj.bottom() + halfH);
            player->fisica().setVelocity(vx, 0.0f);
        }

        // Actualizamos rectJugador para evitar dobles colisiones
        rectJugador.moveCenter(pos);
    }

    // Finalmente, aplicamos la nueva posición calculada
    player->transform().setPosition(pos.x(), pos.y());
}
