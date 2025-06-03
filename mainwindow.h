#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include "entidad.h"

class PantallaInicio;
class MenuOpciones;
class PantallaCarga;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:

    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void onGameLoop();

private:
    QWidget *pantallaActual;
    PantallaInicio *pantallaInicio;
    MenuOpciones *menuOpciones;
    PantallaCarga *pantallaCarga;
    void mostrarPantalla(QWidget *pantalla);
    Ui::MainWindow *ui;

    // Nuestro “jugador” (la entidad que contiene el Sprite, Transformacion, etc.)
    entidad *m_player = nullptr;

    // Timer para game-loop
    QTimer *m_timer = nullptr;

    // Flags de teclas
    bool m_upPressed;
    bool m_downPressed;
    bool m_leftPressed;
    bool m_rightPressed;
    bool m_shiftPressed;
    // Constante delta-time fijo (en segundos)
    const float m_dt = 0.016f;  // ≈60 FPS

    // Función auxiliar para procesar el estado de las teclas y actualizar velocidad de m_player
    void processInput();
};

#endif // MAINWINDOW_H
