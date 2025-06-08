#include "mapawidget.h"
#include <QGraphicsPixmapItem>
#include <QScrollBar>
#include <QDebug>

MapaWidget::MapaWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(950, 650); // Ventana del tamaño del juego

    scene = new QGraphicsScene(this);

    QPixmap fondo(":/resources/mapa.png");
    if (fondo.isNull()) {
        qDebug() << "No se cargó la imagen del mapa.";
        fondo = QPixmap(2000, 1200); // Fondo vacío si falla
        fondo.fill(Qt::darkGray);
    }

    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(fondo);
    scene->addItem(item);
    scene->setSceneRect(0, 0, fondo.width(), fondo.height()); // scroll automático

    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setDragMode(QGraphicsView::ScrollHandDrag); // mover mapa con mouse
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setFixedSize(950, 650);
    view->move(0, 0);
}

void MapaWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Tab) {
        this->close(); // cerrar con TAB
    }
}
