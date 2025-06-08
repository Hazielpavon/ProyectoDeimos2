#ifndef MAPAWIDGET_H
#define MAPAWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QKeyEvent>

class MapaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapaWidget(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
};

#endif // MAPAWIDGET_H
