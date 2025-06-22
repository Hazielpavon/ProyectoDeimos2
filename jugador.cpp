#include "jugador.h"
#include <QDebug>

static constexpr int BAR_WIDTH  = 100;
static constexpr int BAR_HEIGHT = 12;
static constexpr int BAR_MARGIN = 10;

Jugador::Jugador()
    : entidad()
{
    m_baseDamage       = 1;
}

// Añade una llave
void Jugador::addKey(const QString &keyId)
{
    m_keys.insert(keyId);
}

// Comprueba si tiene la llave
bool Jugador::hasKey(const QString &keyId) const
{
    return m_keys.contains(keyId);
}

// Usa (consume) la llave
void Jugador::useKey(const QString &keyId)
{
    m_keys.remove(keyId);
}

// Sobrescribimos para, además de aplicar el daño,
// si vida llega a 0 ponemos animación de muerte
void Jugador::aplicarDano(int dmg)
{
    // primero aplicamos normalmente
    m_componenteSalud.aplicarDano(dmg);

    // si se quedó a 0 => muere
    if (m_componenteSalud.currentHP() == 0) {
        // ponemos el sprite en estado de muerte
        reproducirAnimacionTemporal(SpriteState::dead, 1.0f);
    }
}

// Dibuja la barra de vida en la esquina superior izquierda
void Jugador::drawHUD(QPainter &painter, const QRect &viewportRect) const
{
    // Calculamos posición fija (siempre margen desde top-left)
    int x0 = viewportRect.left() + BAR_MARGIN;
    int y0 = viewportRect.top()  + BAR_MARGIN;

    // Fracción de vida
    float frac = float(m_componenteSalud.currentHP())
                 / float(m_componenteSalud.maxHP());

    // Dibujamos fondo y borde
    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(x0, y0, BAR_WIDTH, BAR_HEIGHT);

    // Barrita gris de "vacío"
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(80,80,80));
    painter.drawRect(x0+1, y0+1, BAR_WIDTH-2, BAR_HEIGHT-2);

    // Relleno verde
    painter.setBrush(QColor(50,205,50));
    painter.drawRect(x0+1, y0+1,
                     int((BAR_WIDTH-2)*frac),
                     BAR_HEIGHT-2);
}
