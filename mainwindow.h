#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// Incluimos los headers de nuestras pantallas, no solo QWidget
#include "pantallainicio.h"
#include "menuopciones.h"
#include "pantallacarga.h"
#include "entidad.h"

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
    // Para capturar teclas y clics de mouse
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainWindow *ui;

    // Declaramos punteros con su tipo concreto, no como QWidget*
    PantallaInicio   *pantallaInicio;
    MenuOpciones     *menuOpciones;
    PantallaCarga    *pantallaCarga;

    QWidget          *pantallaActual; // Único puntero genérico para mostrar/ocultar

    entidad          *m_player;
    QTimer           *m_timer;
    float             m_dt = 1.0f / 60.0f;

    // Flags de entrada
    bool m_leftPressed;
    bool m_rightPressed;
    bool m_shiftPressed;
    bool m_cPressed;

    void mostrarPantalla(QWidget *pant);
    void processInput();

private slots:
    void onGameLoop();
};

#endif // MAINWINDOW_H
