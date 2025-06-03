#include "menuopciones.h"
#include "clickablelabel.h"

#include <QPixmap>
#include <QDebug>
#include <QApplication>

MenuOpciones::MenuOpciones(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(950, 650);

    // Imagen de fondo
    fondo = new QLabel(this);
    fondo->setPixmap(QPixmap(":/resources/menu_Op.png")); // Asegúrate que esté en los recursos
    fondo->setScaledContents(true);
    fondo->resize(950, 650);
    fondo->lower();

    // NUEVA PARTIDA
    ClickableLabel *lblNuevaPartida = new ClickableLabel(this);
    lblNuevaPartida->setGeometry(355, 300, 240, 50);
    lblNuevaPartida->setStyleSheet("background-color: transparent;");
    connect(lblNuevaPartida, &ClickableLabel::clicked, this, [=]() {
        qDebug() << "🎮 NUEVA PARTIDA";
        emit nuevaPartida(); // ✅ Llama la señal para MainWindow
    });

    // CONTINUAR
    ClickableLabel *lblContinuar = new ClickableLabel(this);
    lblContinuar->setGeometry(355, 360, 240, 50);
    lblContinuar->setStyleSheet("background-color: transparent;");
    connect(lblContinuar, &ClickableLabel::clicked, this, [=]() {
        qDebug() << "⏩ CONTINUAR";
        emit continuarJuego();
    });

    // SALIR DEL JUEGO
    ClickableLabel *lblSalir = new ClickableLabel(this);
    lblSalir->setGeometry(355, 420, 240, 50);
    lblSalir->setStyleSheet("background-color: transparent;");
    connect(lblSalir, &ClickableLabel::clicked, this, [=]() {
        qDebug() << "🚪 SALIR DEL JUEGO";
        emit salirDelJuego();
        qApp->exit(); // opcional si también quieres cerrar directo
    });
}
