// CombateManager.h
#pragma once
#include <QObject>
#include <QVector>
#include <QRectF>

class Jugador;
class Enemigo;

class CombateManager : public QObject {
    Q_OBJECT
public:
    CombateManager(Jugador* player,
                   QVector<Enemigo*>& enemigos,
                   QObject* parent = nullptr);

    void update(float dt);

private:
    Jugador*           m_player    = nullptr;
    QVector<Enemigo*>& m_enemigos;            // referencia al vector del Nivel
    float               m_playerCd  = 0.0f;   // ← inicializado
    float               m_enemyCd   = 0.0f;

    // helpers de hit-box
    QRectF espadaRect(Jugador* j) const;
    QRectF garraRect(Enemigo*  e) const;
};
