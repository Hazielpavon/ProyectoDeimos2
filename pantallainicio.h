#ifndef PANTALLAINICIO_H
#define PANTALLAINICIO_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class PantallaInicio : public QWidget
{
    Q_OBJECT

public:
    explicit PantallaInicio(QWidget *parent = nullptr);

signals:
    void iniciarJuegoPresionado();

private:
    QLabel *fondoLabel;
    QPushButton *botonIniciar;
};

#endif // PANTALLAINICIO_H
