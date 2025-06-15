#include "enemigo.h"
#include "entidad.h"
#include <QDebug>

/* ---------- Constructor ---------- */
Enemigo::Enemigo(QObject* parent)
    : QObject(parent), QGraphicsPixmapItem()
{
    setFlag(ItemIsMovable, false);
    setZValue(2.5);
}

/* ---------- Animaciones helpers ---------- */
void Enemigo::addAnim(Estado st, const Animacion& a)
{
    m_anims.insert(st, a);
    if (st == m_estado && !a.frames.isEmpty())
        setPixmap(a.frames.first());
}

Animacion& Enemigo::animActual()
{
    return m_anims[m_estado];
}
const Animacion& Enemigo::animActual() const
{
    return m_anims[m_estado];
}

void Enemigo::setEstado(Estado st)
{
    if (m_estado == st) return;
    m_estado = st;
    auto& a = m_anims[m_estado];
    a.idx  = 0;
    a.acum = 0.0f;
    if (!a.frames.isEmpty())
        setPixmap(a.frames.first());
}

/* ---------- Salud ---------- */
void Enemigo::takeDamage(int dmg)
{
    if (isDead()) return;
    m_hp -= dmg;
    if (m_hp <= 0) {
        m_hp = 0;
        setEstado(Estado::Death);
        m_velX = m_velY = 0.0f;
    } else {
        setEstado(Estado::Hurt);
    }
}
