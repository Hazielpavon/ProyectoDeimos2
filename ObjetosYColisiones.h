#ifndef OBJETOSYCOLISIONES_H
#define OBJETOSYCOLISIONES_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QColor>
#include <QRectF>
#include <vector>

class entidad;

class ObjetosYColisiones : public QObject
{
    Q_OBJECT
public:
    explicit ObjetosYColisiones(QGraphicsScene* scene, QObject* parent = nullptr);

    QGraphicsRectItem* addRect(const QRectF& area,
                               const QColor& color,
                               bool collisionOnly);

    // Ahora el hitbox usa pixSize.width() y pixSize.height() sin escalado
    void resolveCollisions(entidad* player,
                           const QSize& pixSize,
                           float dt);

private:
    struct Obj { QGraphicsRectItem* vis; QGraphicsRectItem* hitbox; };
    std::vector<Obj> m_objetos;
    QGraphicsScene*  m_scene;
};

#endif // OBJETOSYCOLISIONES_H
