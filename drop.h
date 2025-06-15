// drop.h
#pragma once
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QString>
#include "entidad.h"

class Drop
{
public:
    enum class Tipo { Vida, Mana, Llave };

    Drop(Tipo tipo, const QPointF& posicion, QGraphicsScene* scene, const QString& nombreLlave = "");
    ~Drop();

    void aplicarEfecto(entidad* player);
    bool checkCollision(entidad* player);
    bool isCollected() const;

    Tipo tipo() const;
    QString nombreLlave() const;

private:
    Tipo m_tipo;
    QString m_nombreLlave;
    QGraphicsPixmapItem* m_sprite = nullptr;
    bool m_collected = false;
};
