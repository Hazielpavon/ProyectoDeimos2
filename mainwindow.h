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
    void mostrarPantalla(QWidget *pantalla);
     void mostrarNivelConIntro(const QString &rutaVideo, QWidget *nivel);
    void cargarNivel(const QString &nombre);
private:
    Ui::MainWindow *ui;

    entidad *m_player;
    QTimer  *m_timer;

    bool m_upPressed;
    bool m_downPressed;
    bool m_leftPressed;
    bool m_rightPressed;
    bool m_shiftPressed;

    const float m_dt = 0.016f;
    void paintEvent(QPaintEvent * );
    QWidget *pantallaActual;
    PantallaInicio *pantallaInicio;
    MenuOpciones   *menuOpciones;
    PantallaCarga  *pantallaCarga;


};

#endif // MAINWINDOW_H
