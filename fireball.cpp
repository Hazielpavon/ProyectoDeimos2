#include "fireball.h"
#include <QDebug>

Fireball::Fireball(bool haciaIzquierda, QPointF origen, QGraphicsScene* scene, QVector<Enemigo*>& enemigos)
    : QGraphicsPixmapItem(), m_scene(scene), m_enemigos(enemigos)
{
    // Cargar 17 frames desde recursos
    for (int i = 1; i < 17; ++i) {
        QString framePath = QString(":/resources/Bringer-of-Death_Spell_%1.png").arg(i);
        QPixmap pix(framePath);
        if (pix.isNull()) {
            qWarning() << "No se pudo cargar:" << framePath;
        }
        frames.append(pix);
    }

    if (!frames.isEmpty()) {
        setPixmap(frames[0]);
        setOffset(-frames[0].width() / 2.0, -frames[0].height() / 2.0);
        setZValue(5);
    }

    setPos(origen.x(), origen.y() - 30);
    velX = haciaIzquierda ? -400.0f : 400.0f;
    m_scene->addItem(this);
}

void Fireball::avanzar(float dt)
{
    if (!m_alive) return;

    moveBy(velX * dt, 0);

    tiempoAcumulado += dt;
    if (tiempoAcumulado >= 0.05f && !frames.isEmpty()) {
        frameIndex = (frameIndex + 1) % frames.size();
        setPixmap(frames[frameIndex]);
        tiempoAcumulado = 0;
    }

    // Colisión con enemigos
    for (Enemigo* e : m_enemigos) {
        if (e && !e->isDead() && collidesWithItem(e)) {
            e->takeDamage(4);
            m_scene->removeItem(this);
            m_alive = false;
            deleteLater();
            return;
        }
    }

    // Si sale de la escena
    if (x() < 0 || x() > m_scene->sceneRect().width()) {
        m_scene->removeItem(this);
        m_alive = false;
        deleteLater();
    }
}
