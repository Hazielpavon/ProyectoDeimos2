#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include "entidad.h"

/* ----- forward declarations de tus pantallas ----- */
class PantallaInicio;
class MenuOpciones;
class PantallaCarga;
class TutorialScene;
class niveltorredelamarca;
class ciudadinversa;
class NivelRaicesOlvidadas;

/* ----- inventario ---- */
class InventoryWidget;

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

protected:
    /* ← tecla I para el inventario */
    void keyPressEvent(QKeyEvent* ev) override;
    void paintEvent  (QPaintEvent*)   override;

private:
    Ui::MainWindow *ui;

    entidad  *m_player = nullptr;
    QTimer   *m_timer  = nullptr;

    /* flags de entrada globales (usados por tus menús) */
    bool m_upPressed   = false;
    bool m_downPressed = false;
    bool m_leftPressed = false;
    bool m_rightPressed= false;
    bool m_shiftPressed= false;

    const float m_dt = 0.016f;

    /* pantallas */
    QWidget        *pantallaActual = nullptr;
    PantallaInicio *pantallaInicio = nullptr;
    MenuOpciones   *menuOpciones   = nullptr;
    PantallaCarga  *pantallaCarga  = nullptr;

    /* inventario (única instancia) */
    InventoryWidget* m_inventario  = nullptr;
};

#endif // MAINWINDOW_H
