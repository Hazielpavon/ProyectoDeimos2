#pragma once
#include "entidad.h"
#include <QPainter>
#include <QString>
#include <QSet>
#include <QDebug>

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

    // Dibuja la HUD (barra de vida) en la esquina superior izquierda
    void drawHUD(QPainter &painter, const QRect &viewportRect) const;

private:
    QSet<QString>  m_keys;
};
