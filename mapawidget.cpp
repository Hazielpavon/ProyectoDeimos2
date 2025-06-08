#include "mapawindow.h"
#include <QGraphicsPixmapItem>
#include <QScrollBar>
#include <QDebug>

MapaWindow::MapaWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(950, 650); // Tamaño de ventana como el juego

    scene = new QGraphicsScene(this);

    QPixmap fondo(":/resources/mapa_regiones.png");
    if (fondo.isNull()) {
        qDebug() << "No se cargó la imagen del mapa.";
        fondo = QPixmap(2000, 1200); // Imagen de respaldo vacía
        fondo.fill(Qt::darkGray);
    }

    // Insertamos imagen de tamaño más grande en la escena
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(fondo);
    scene->addItem(item);
    scene->setSceneRect(0, 0, fondo.width(), fondo.height()); // Extensión de scroll

    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setDragMode(QGraphicsView::ScrollHandDrag); // mover con click arrastrando
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setFixedSize(950, 650);

    setCentralWidget(view);
}

void MapaWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Tab) {
        this->close(); // cerrar ventana si vuelve a presionar TAB
    }
}
