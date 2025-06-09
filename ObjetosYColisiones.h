// ObjetosYColisiones.h
#pragma once

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <vector>

class entidad;

class ObjetosYColisiones : public QObject
{
    Q_OBJECT
public:
    explicit ObjetosYColisiones(QGraphicsScene* scene, QObject* parent = nullptr);

    QGraphicsRectItem* addRect(const QRectF& area,
                               const QColor& color = QColor(80,80,80),
                               bool collisionOnly = false);
    void resolveCollisions(entidad* player, float dt);

private:
    struct Objeto { QGraphicsRectItem* visual; QGraphicsRectItem* hitbox; };
    std::vector<Objeto> m_objetos;
    QGraphicsScene*     m_scene;
};
