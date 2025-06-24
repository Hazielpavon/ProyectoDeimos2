#include "jugador.h"
#include <QtMath>

static constexpr int BAR_W = 100;
static constexpr int BAR_H = 12;
static constexpr int BAR_M = 10;

Jugador::Jugador()
    : entidad()
{
}

void Jugador::addKey(const QString& id)  { m_keys.insert(id); }
bool Jugador::hasKey(const QString& id) const { return m_keys.contains(id); }
void Jugador::useKey(const QString& id)  { m_keys.remove(id); }

void Jugador::aplicarDano(int dmg)
{
    salud().aplicarDano(dmg);

    if (currentHP() == 0)
        reproducirAnimacionTemporal(SpriteState::dead, 1.0f);
}

void Jugador::drawHUD(QPainter& p, const QRect& vp) const
{
    int x0 = vp.left() + BAR_M;
    int y0 = vp.top()  + BAR_M;

    float frac = float(currentHP()) / float(maxHP());

    p.setPen(Qt::black);  p.setBrush(Qt::NoBrush);
    p.drawRect(x0, y0, BAR_W, BAR_H);

    p.setPen(Qt::NoPen);  p.setBrush(QColor(80,80,80));
    p.drawRect(x0+1, y0+1, BAR_W-2, BAR_H-2);

    p.setBrush(QColor(50,205,50));
    p.drawRect(x0+1, y0+1, int((BAR_W-2)*frac), BAR_H-2);
}
