#ifndef MAPAWIDGET_H
#define MAPAWIDGET_H

#include <QWidget>

class MapaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapaWidget(QWidget *parent = nullptr);

signals:
    void zonaSeleccionada(const QPoint &puntoMapa);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // MAPAWIDGET_H
