#pragma once
#include <QObject>
#include <QGraphicsRectItem>
#include <QVector>
#include <QSize>

struct ColObj {
    QGraphicsRectItem* visual;
    QGraphicsRectItem* hitbox;
};

class entidad;      // forward
class Enemigo;     // forward

class ObjetosYColisiones : public QObject {
    Q_OBJECT
public:
    explicit ObjetosYColisiones(QGraphicsScene* scene, QObject* parent = nullptr);

    // añade un rect (hitbox invisible + opcional visual)
    QGraphicsRectItem* addRect(const QRectF& area,
                               const QColor& color = Qt::NoBrush,
                               bool collisionOnly = true);

    // colisión para jugador
    void resolveCollisions(entidad* player,
                           const QSize& pixSize,
                           float dt);

    // colisión para enemigo
    void resolveCollisions(Enemigo* e,
                           const QSize& pixSize,
                           float dt);

private:
    QGraphicsScene* m_scene;
    QVector<ColObj> m_objetos;

    // factores ajustados
    static constexpr float HITBOX_W_FACTOR_P = 0.40f;
    static constexpr float HITBOX_H_FACTOR_P = 0.71f;
    static constexpr float HITBOX_W_FACTOR_E = 0.40f;
    static constexpr float HITBOX_H_FACTOR_E = 0.60f;
};
