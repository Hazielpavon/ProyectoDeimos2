#include "componentesalud.h"
#include <QPainter>
#include <QRect>

componentesalud::componentesalud()
    : m_currentHP(100),
    m_maxHP(100)
{
}

void componentesalud::setHP(int hp)
{
    m_currentHP = qBound(0, hp, m_maxHP);
}

int componentesalud::currentHP() const
{
    return m_currentHP;
}

int componentesalud::maxHP() const
{
    return m_maxHP;
}

void componentesalud::aplicarDano(int dmg)
{
    m_currentHP = qMax(0, m_currentHP - dmg);
}

void componentesalud::actualizar(float /*dt*/)
{
    // Aquí podrías regenerar vida, temporizadores, etc.
}

void componentesalud::dibujar(QPainter &painter, const QPoint &posSprite)
{
    const int barraAncho  = 64;
    const int barraAlto   = 8;
    const int margenY     = 10;

    float fraccionVida = 0.0f;
    if (m_maxHP > 0)
        fraccionVida = float(m_currentHP) / float(m_maxHP);

    int xBarra = posSprite.x() + 0 - (barraAncho / 2) + 32;
    int yBarra = posSprite.y() - margenY;

    QRect rectBorde(xBarra, yBarra, barraAncho, barraAlto);
    QRect rectVida (xBarra + 1, yBarra + 1,int((barraAncho - 2) * fraccionVida), barraAlto - 2);

    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rectBorde);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(80, 80, 80));
    painter.drawRect(rectBorde.adjusted(1, 1, -1, -1));

    painter.setBrush(QColor(50, 205, 50));
    painter.drawRect(rectVida);


}
