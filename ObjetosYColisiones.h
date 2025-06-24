#pragma once
#include <QObject>
#include <QGraphicsRectItem>
#include <QVector>
#include <QSize>

struct ColObj {
    QGraphicsRectItem* visual;
    QGraphicsRectItem* hitbox;
};

class entidad;
class Enemigo;

class ObjetosYColisiones : public QObject {
    Q_OBJECT
public:
    explicit ObjetosYColisiones(QGraphicsScene* scene, QObject* parent = nullptr);

    QGraphicsRectItem* addRect(const QRectF& area,
                               const QColor& color = Qt::NoBrush,
                               bool collisionOnly = true);

    void resolveCollisions(entidad* player,
                           const QSize& pixSize,
                           float dt);

    void resolveCollisions(Enemigo* e,
                           const QSize& pixSize,
                           float dt);
    void removeRect(QGraphicsRectItem* hitbox);
private:
    QGraphicsScene* m_scene;
    QVector<ColObj> m_objetos;

    static constexpr float HITBOX_W_FACTOR_P = 0.40f;
    static constexpr float HITBOX_H_FACTOR_P = 0.71f;
    static constexpr float HITBOX_W_FACTOR_E = 0.40f;
    static constexpr float HITBOX_H_FACTOR_E = 0.60f;
};
