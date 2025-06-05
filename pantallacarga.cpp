#include "PantallaCarga.h"
#include <QPixmap>

PantallaCarga::PantallaCarga(QWidget *parent)
    : QWidget(parent), progreso(0) {
    setFixedSize(950, 650);
    fondo = new QLabel(this);
    fondo->setPixmap(QPixmap(":/resources/cargando.png"));
    fondo->setScaledContents(true);
    fondo->resize(size());
    barra = new QProgressBar(this);
    barra->setGeometry(275, 610, 400, 25);
    barra->setRange(0, 100);
    barra->setValue(0);
    barra->setStyleSheet("QProgressBar { background-color: #333; color: white; border: 2px solid gray; border-radius: 5px; }""QProgressBar::chunk { background-color: #5cb85c; width: 10px; margin: 1px; }");
    barra->raise();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &PantallaCarga::actualizarBarra);
    timer->start(80);
}
void PantallaCarga::actualizarBarra() {
    if (progreso < 100) {
        progreso += 2;
        barra->setValue(progreso);
    } else {
        timer->stop();
        emit cargaCompletada();
    }
}
