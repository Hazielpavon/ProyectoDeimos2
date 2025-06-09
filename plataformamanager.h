#ifndef PLATAFORMAMANAGER_H
#define PLATAFORMAMANAGER_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QVector>

class PlataformaManager {
public:
    explicit PlataformaManager(QGraphicsScene* scene);
    ~PlataformaManager();

    void crearNuevaPlataforma(qreal posX, qreal posY, qreal ancho, qreal alto);

    QVector<QGraphicsRectItem*> colisionItems() const;

private:
    QGraphicsScene* m_scene;
    QVector<QGraphicsRectItem*> m_visualItems;
    QVector<QGraphicsRectItem*> m_colisionItems;
};

#endif
