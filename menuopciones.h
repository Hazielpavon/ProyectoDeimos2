#ifndef MENUOPCIONES_H
#define MENUOPCIONES_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class MenuOpciones : public QWidget
{
    Q_OBJECT

public:
    explicit MenuOpciones(QWidget *parent = nullptr);

signals:
    void nuevaPartida();
    void continuarJuego();
    void salirDelJuego();

private:
    QLabel *fondo;
    QPushButton *btnNuevaPartida;
    QPushButton *btnContinuar;
    QPushButton *btnSalir;
};

#endif // MENUOPCIONES_H
