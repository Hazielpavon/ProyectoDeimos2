#ifndef MAPAWINDOW_H
#define MAPAWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QKeyEvent>

class MapaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MapaWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
};

#endif // MAPAWINDOW_H
