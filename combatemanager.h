#pragma once
#include <QObject>
#include <QVector>
#include <QRectF>
#include <QDebug>

class Jugador;
class Enemigo;

class CombateManager : public QObject
{
    Q_OBJECT
public:
    CombateManager(Jugador* player,
                   QVector<Enemigo*>& enemigos,
                   QObject* parent=nullptr);

    void update(float dt);

private:
    QRectF espadaRect(Jugador*  j) const;
    QRectF garraRect (Enemigo*  e) const;

    Jugador*            m_player;
    QVector<Enemigo*>&  m_enemigos;

    float m_playerCd = 0.0f;
    float m_enemyCd  = 0.0f;
};
// VIVA EL SEXO HDP
//testing
