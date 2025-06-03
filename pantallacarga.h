#ifndef PANTALLACARGA_H
#define PANTALLACARGA_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>

class PantallaCarga : public QWidget {
    Q_OBJECT

public:
    explicit PantallaCarga(QWidget *parent = nullptr);

signals:
    void cargaCompletada();

private:
    QLabel *fondo;
    QProgressBar *barra;
    QTimer *timer;
    int progreso;

private slots:
    void actualizarBarra();
};

#endif // PANTALLACARGA_H
