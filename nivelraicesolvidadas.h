#ifndef NIVELRAICESOLVIDADAS_H
#define NIVELRAICESOLVIDADAS_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

class NivelRaicesOlvidadas : public QWidget
{
    Q_OBJECT

public:
    explicit NivelRaicesOlvidadas(QWidget *parent = nullptr);

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    int fondoAncho;
    int fondoAlto;
};

#endif // NIVELRAICESOLVIDADAS_H
