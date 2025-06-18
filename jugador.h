#pragma once
#include "entidad.h"
#include <QPainter>
#include <QString>
#include <QSet>
#include <QDebug>
#include <QGraphicsPixmapItem>  // QGraphicsPixmapItem, sceneBoundingRect()
#include <QRectF>                // QRectF

class Jugador : public entidad
{
public:
    Jugador();

    // Inventario de llaves
    void addKey(const QString &keyId);
    bool hasKey(const QString &keyId) const;
    void useKey(const QString &keyId);

    // Override aplicación de daño para disparar animación de muerte
    void aplicarDano(int dmg);

    void setGraphicsItem(QGraphicsPixmapItem* it) { m_graphicsItem = it; }
    // Lo usamos luego en CombateManager:
    QGraphicsPixmapItem* graphicsItem() const { return m_graphicsItem; }


    // Dibuja la HUD (barra de vida) en la esquina superior izquierda
    void drawHUD(QPainter &painter, const QRect &viewportRect) const;
    void   setDamageMultiplier(float m) { m_damageMul = m; }
    float  damageMultiplier() const     { return m_damageMul; }
    int    computeDamage(int base) const {
        return int(std::round(base * m_damageMul));
    }
private:
    QGraphicsPixmapItem* m_graphicsItem = nullptr;
    QSet<QString>  m_keys;
    float  m_damageMul = 1.0f;
};
