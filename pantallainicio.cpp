#include "pantallainicio.h"
#include <QPixmap>
#include <QDebug>

PantallaInicio::PantallaInicio(QWidget *parent)
    : QWidget(parent)
{
    qDebug() << "🟢 PantallaInicio cargada";

    setFixedSize(950, 650);

    fondoLabel = new QLabel(this);
    fondoLabel->setPixmap(QPixmap(":/resources/inicio.png")); // Imagen de DEIMOS + botón rojo
    fondoLabel->setScaledContents(true);
    fondoLabel->resize(950, 650);
    fondoLabel->lower();

    botonIniciar = new QPushButton("INICIAR JUEGO", this);
    botonIniciar->setGeometry(520, 400, 200, 40);
    botonIniciar->setStyleSheet("background-color: #AA0000; color: white; font-size: 18px; border-radius: 10px;");
    botonIniciar->raise();

    connect(botonIniciar, &QPushButton::clicked, this, &PantallaInicio::iniciarJuegoPresionado);
}
