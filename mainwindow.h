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
class niveltorredelamarca;
class ciudadinversa;
class NivelRaicesOlvidadas;
class mentevacia;
class InventoryWidget;
class SkillTreeWidget;

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
    void cargarNivel   (const QString &nombre);
    void mostrarNivelConIntro(const QString &rutaVideo, QWidget *nivel);

protected:
    void keyPressEvent(QKeyEvent *ev) override;
    void paintEvent  (QPaintEvent *)   override;

private:
    void toggleInventory();
    void toggleSkillTree();
    Ui::MainWindow    *ui            = nullptr;
    entidad           *m_player      = nullptr;
    QTimer            *m_timer       = nullptr;
    QWidget           *pantallaActual = nullptr;
    PantallaInicio    *pantallaInicio = nullptr;
    MenuOpciones      *menuOpciones   = nullptr;
    PantallaCarga     *pantallaCarga  = nullptr;
    InventoryWidget   *m_inventario  = nullptr;
    SkillTreeWidget   *m_skillTree   = nullptr;
};

#endif // MAINWINDOW_H
