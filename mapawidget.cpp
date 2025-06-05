#include "mapawidget.h"
#include <QPainter>
#include <QMouseEvent>

MapaWidget::MapaWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(200, 150);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
}

void MapaWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Fondo semitransparente
    painter.setBrush(QColor(30, 30, 30, 200));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 10, 10);

    // Zonas interactivas (puedes modificarlas)
    painter.setBrush(QColor(0, 120, 255, 180));
    painter.drawRect(30, 30, 40, 40); // Zona 1
    painter.setBrush(QColor(255, 100, 0, 180));
    painter.drawRect(120, 60, 40, 40); // Zona 2
}

void MapaWidget::mousePressEvent(QMouseEvent *event)
{
    emit zonaSeleccionada(event->pos());
}
