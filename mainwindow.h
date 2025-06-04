#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include "entidad.h"

class PantallaInicio;
class MenuOpciones;
class PantallaCarga;
class TutorialScene;

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
    Ui::MainWindow *ui;

    entidad *m_player;
    QTimer  *m_timer;

    bool m_upPressed;
    bool m_downPressed;
    bool m_leftPressed;
    bool m_rightPressed;
    bool m_shiftPressed;

    const float m_dt = 0.016f; // ~60 FPS

    QWidget *pantallaActual;
    PantallaInicio *pantallaInicio;
    MenuOpciones   *menuOpciones;
    PantallaCarga  *pantallaCarga;

    void processInput();
    void mostrarPantalla(QWidget *pantalla);
};

#endif // MAINWINDOW_H
