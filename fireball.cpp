#include "fireball.h"
#include "enemigo.h"
#include <QGraphicsScene>
#include <QDebug>

Fireball::Fireball(bool haciaIzquierda,
                   QPointF origen,
                   QGraphicsScene* scene,
                   QVector<Enemigo*>* enemigos)
    : QGraphicsPixmapItem(nullptr)
    , m_scene(scene)
    , m_enemigos(enemigos)
    , m_alive(true)
    , velX(haciaIzquierda ? -400.0f : 400.0f)
{
    // Carga de los frames
    for (int i = 1; i < 17; ++i) {
        QString path = QString(":/resources/Bringer-of-Death_Spell_%1.png").arg(i);
        QPixmap px(path);
        if (px.isNull()) qWarning() << "No se pudo cargar" << path;
        frames.append(px);
    }
    if (!frames.isEmpty()) {
        setPixmap(frames[0]);
        setOffset(-frames[0].width()/2.0, -frames[0].height()/2.0);
        setZValue(5);
    }
    setPos(origen.x(), origen.y() - 30);
    m_scene->addItem(this);
}

void Fireball::avanzar(float dt)
{
    if (!m_alive) return;

    // 1) Movimiento
    moveBy(velX * dt, 0);

    // 2) Animación
    tiempoAcumulado += dt;
    if (tiempoAcumulado >= 0.05f && !frames.isEmpty()) {
        frameIndex = (frameIndex + 1) % frames.size();
        setPixmap(frames[frameIndex]);
        tiempoAcumulado = 0;
    }

    // 3) Colisión con enemigos VIVOS en la lista original
    if (m_enemigos) {
        for (Enemigo* e : *m_enemigos) {
            if (!e) continue;
            if (!e->isDead() && collidesWithItem(e)) {
                e->takeDamage(4);
                m_scene->removeItem(this);
                m_alive = false;
                return;
            }
        }
    }

    // 4) Si sale de la escena
    QRectF r = m_scene->sceneRect();
    if (x() < r.left() || x() > r.right()) {
        m_scene->removeItem(this);
        m_alive = false;
    }
}

