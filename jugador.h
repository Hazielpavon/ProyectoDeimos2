
#pragma once
#include "entidad.h"

#include <QPainter>
#include <QSet>
#include <QString>
#include <QGraphicsPixmapItem>
#include <cmath>

class Jugador : public entidad
{
public:
    Jugador();
    void addKey   (const QString& keyId);
    bool hasKey   (const QString& keyId) const;
    void useKey   (const QString& keyId);

    void aplicarDano(int dmg);
    void setDamageMultiplier(float m) { m_damageMul = m; }
    float damageMultiplier() const      { return m_damageMul; }

    int  damage()              const { return m_baseDamage; }
    int  computeDamage(int base)const { return int(std::round(base * m_damageMul)); }

    void drawHUD(QPainter& painter, const QRect& viewportRect) const;

    void setGraphicsItem(QGraphicsPixmapItem* it)   { m_graphicsItem = it; }
    QGraphicsPixmapItem* graphicsItem() const       { return m_graphicsItem; }

private:

    int   m_baseDamage  = 1;
    float m_damageMul   = 1.0f;

    QSet<QString> m_keys;

    QGraphicsPixmapItem* m_graphicsItem = nullptr;
};
