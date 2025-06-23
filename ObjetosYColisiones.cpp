#include "ObjetosYColisiones.h"
#include "entidad.h"
#include "Enemigo.h"
#include "jugador.h"
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
// --- en ObjetosYColisiones.cpp ---
void ObjetosYColisiones::removeRect(QGraphicsRectItem* hitboxToRemove)
{
    for (auto it = m_objetos.begin(); it != m_objetos.end(); ++it) {
        if (it->hitbox == hitboxToRemove) {
            // 1) borrar el visual, si lo hay
            if (it->visual) {
                m_scene->removeItem(it->visual);
                delete it->visual;
            }
            // 2) borrar el hitbox
            m_scene->removeItem(it->hitbox);
            delete it->hitbox;
            // 3) quitar del vector
            m_objetos.erase(it);
            return;
        }
    }
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

        // ——— 1) Colisión horizontal ———
        bool forceHorz = false;
        if (auto* jug = dynamic_cast<Jugador*>(player)) {
            // sprite().getState() devuelve el estado actual
            if (jug->sprite().getState() == SpriteState::Idle)
                forceHorz = true;
        }

        // moviéndonos a la derecha, o en Idle forzado
        if (vx > 0 || (vx == 0 && forceHorz)) {
            float right = footPos.x() + halfW;
            if ( right > rObj.left()
                && (footPos.x() - halfW) < rObj.left() )
            {
                footPos.setX( rObj.left() - halfW );
                player->fisica().setVelocity(0, vy);
                vx = 0;
                rectJ.moveLeft(footPos.x() - halfW);
            }
        }
        // moviéndonos a la izquierda, o en Idle forzado
        else if (vx < 0 || (vx == 0 && forceHorz)) {
            float left = footPos.x() - halfW;
            if ( left < rObj.right()
                && (footPos.x() + halfW) > rObj.right() )
            {
                footPos.setX( rObj.right() + halfW );
                player->fisica().setVelocity(0, vy);
                vx = 0;
                rectJ.moveLeft(footPos.x() - halfW);
            }
        }

        // ——— 2) Colisión vertical ———
        float pieY    = footPos.y();
        float cabezaY = pieY - hitH;

        // aterrizar
        if (vy >= 0 && pieY >= rObj.top() && cabezaY < rObj.top()) {
            footPos.setY(rObj.top());
            player->fisica().setVelocity(vx, 0);
            player->setOnGround(true);
            rectJ.moveTop( footPos.y() - hitH );
        }
        // choque de cabeza
        else if (vy < 0 && cabezaY <= rObj.bottom() && pieY > rObj.bottom()) {
            footPos.setY(rObj.bottom() + hitH);
            player->fisica().setVelocity(vx, 0);
            rectJ.moveTop( footPos.y() - hitH );
        }
    }

    player->transform().setPosition(footPos.x(), footPos.y());
}

void ObjetosYColisiones::resolveCollisions(Enemigo* e,const QSize& pixSize,float /*dt*/)
{
    if (!e) return;

    // 1) Calculamos pies y centro
    QPointF center = e->pos();
    float halfH = pixSize.height() * 0.5f;
    QPointF footPos(center.x(), center.y() + halfH);

    // 2) Velocidades y hitbox enemigo
    float vx = e->velX();
    float vy = e->velY();

    float hitW  = pixSize.width()  * HITBOX_W_FACTOR_E;
    float hitH  = pixSize.height() * HITBOX_H_FACTOR_E;
    float halfW = hitW * 0.5f;

    QRectF rectE(footPos.x() - halfW,
                 footPos.y() - hitH,
                 hitW, hitH);

    // 3) Recorremos cada objeto estático
    for (auto& obj : m_objetos) {
        QRectF rObj = obj.hitbox->sceneBoundingRect();
        if (!rectE.intersects(rObj)) continue;

        // ——— Colisión HORIZONTAL ———
        if (vx > 0) {
            // moviéndose a la derecha
            float right = footPos.x() + halfW;
            if ( right > rObj.left()
                && (footPos.x() - halfW) < rObj.left() )
            {
                // ajustamos pie a la izquierda del bloque
                footPos.setX(rObj.left() - halfW);
                e->setVelX(0);
                vx = 0;
                rectE.moveLeft(footPos.x() - halfW);
            }
        }
        else if (vx < 0) {
            // moviéndose a la izquierda
            float left = footPos.x() - halfW;
            if ( left < rObj.right()
                && (footPos.x() + halfW) > rObj.right() )
            {
                // ajustamos pie a la derecha del bloque
                footPos.setX(rObj.right() + halfW);
                e->setVelX(0);
                vx = 0;
                rectE.moveLeft(footPos.x() - halfW);
            }
        }

        // ——— Colisión VERTICAL ———
        float pieY    = footPos.y();
        float cabezaY = pieY - hitH;

        // aterrizar sobre la parte superior
        if (vy >= 0 && pieY >= rObj.top() && cabezaY < rObj.top()) {
            footPos.setY(rObj.top());
            e->setVelY(0);
        }
        // chocar la cabeza contra la parte inferior
        else if (vy < 0 && cabezaY <= rObj.bottom() && pieY > rObj.bottom()) {
            footPos.setY(rObj.bottom() + hitH);
            e->setVelY(0);
        }

        // avanzamos el rect hacia la nueva posición de pies
        rectE.moveTopLeft({footPos.x() - halfW, footPos.y() - hitH});
    }

    // 4) Reubicamos el sprite de enemigo
    e->setPos(footPos.x(), footPos.y() - halfH);
}


