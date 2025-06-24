#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include "entidad.h"

/* ---------- forward-declarations de pantallas / niveles ---------- */
class PantallaInicio;
class MenuOpciones;
class PantallaCarga;
class TutorialScene;
class niveltorredelamarca;
class ciudadinversa;
class NivelRaicesOlvidadas;
class mentevacia;

/* ---------- widgets auxiliares ---------- */
class InventoryWidget;
class SkillTreeWidget;          // árbol de habilidades

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /* navegación entre pantallas y niveles */
    void mostrarPantalla(QWidget *pantalla);
    void cargarNivel   (const QString &nombre);
    void mostrarNivelConIntro(const QString &rutaVideo, QWidget *nivel);   // si lo sigues usando

protected:
    /* I → inventario | U → árbol de habilidades */
    void keyPressEvent(QKeyEvent *ev) override;
    void paintEvent  (QPaintEvent *)   override;

private:
    /* helpers internos para mostrar / ocultar */
    void toggleInventory();
    void toggleSkillTree();

    /* ---------- miembros ---------- */
    Ui::MainWindow    *ui            = nullptr;

    /* juego */
    entidad           *m_player      = nullptr;
    QTimer            *m_timer       = nullptr;

    /* pantallas */
    QWidget           *pantallaActual = nullptr;
    PantallaInicio    *pantallaInicio = nullptr;
    MenuOpciones      *menuOpciones   = nullptr;
    PantallaCarga     *pantallaCarga  = nullptr;

    /* widgets flotantes (instancias únicas) */
    InventoryWidget   *m_inventario  = nullptr;
    SkillTreeWidget   *m_skillTree   = nullptr;
};

#endif // MAINWINDOW_H
