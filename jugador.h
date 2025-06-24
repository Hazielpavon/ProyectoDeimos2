/* ===========================================================
 *  Jugador.h  —  Clase derivada de entidad que representa al
 *                personaje controlado por el usuario.
 * =========================================================== */
#pragma once
#include "entidad.h"

#include <QPainter>
#include <QSet>
#include <QString>
#include <QGraphicsPixmapItem>
#include <cmath>                   // std::round

class Jugador : public entidad
{
public:
    Jugador();

    /* ---------- Inventario de llaves ---------- */
    void addKey   (const QString& keyId);
    bool hasKey   (const QString& keyId) const;
    void useKey   (const QString& keyId);

    /* ---------- Salud / daño ---------- */
    void aplicarDano(int dmg);                     // aplica daño + animación
    void setDamageMultiplier(float m) { m_damageMul = m; }
    float damageMultiplier() const      { return m_damageMul; }

    int  damage()              const { return m_baseDamage; }
    int  computeDamage(int base)const { return int(std::round(base * m_damageMul)); }

    /* ---------- HUD ---------- */
    void drawHUD(QPainter& painter, const QRect& viewportRect) const;

    /* ---------- Enlace gráfico para colisiones ---------- */
    void setGraphicsItem(QGraphicsPixmapItem* it)   { m_graphicsItem = it; }
    QGraphicsPixmapItem* graphicsItem() const       { return m_graphicsItem; }

private:
    /*  stats */
    int   m_baseDamage  = 1;        // daño base (antes de multiplicador)
    float m_damageMul   = 1.0f;     // multiplicador (mejoras de skill-tree)

    /*  inventario sencillo de llaves */
    QSet<QString> m_keys;

    /*  puntero al item de la escena (lo usa CombateManager p.ej.) */
    QGraphicsPixmapItem* m_graphicsItem = nullptr;
};
