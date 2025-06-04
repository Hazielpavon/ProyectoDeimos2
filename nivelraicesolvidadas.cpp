#include "nivelraicesolvidadas.h"
#include <QPixmap>
#include <QVBoxLayout>
#include <QGraphicsPixmapItem>

NivelRaicesOlvidadas::NivelRaicesOlvidadas(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(950, 650);

    QPixmap fondo(":/resources/raices_olvidadas.png");
    fondoAncho = fondo.width();
    fondoAlto = fondo.height();
    const int repeticiones = 3;

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, fondoAncho * repeticiones, fondoAlto);

    for (int i = 0; i < repeticiones; ++i) {
        QGraphicsPixmapItem *fondoItem = new QGraphicsPixmapItem(fondo);
        fondoItem->setPos(i * fondoAncho, 0);
        fondoItem->setZValue(0);
        scene->addItem(fondoItem);
    }

    view = new QGraphicsView(scene, this);
    view->setFixedSize(950, 650);
    view->setSceneRect(0, 0, 950, 650);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(view);
    setLayout(layout);
}
