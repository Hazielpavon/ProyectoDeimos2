#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pantallainicio.h"
#include "menuopciones.h"
#include "pantallacarga.h"
#include "tutorialscene.h"
#include "nivelraicesolvidadas.h"
#include "niveltorredelamarca.h"
#include "ciudadinversa.h"
#include "mentevacia.h"
#include "maquina_olvido.h"
#include "jugador.h"
#include "InventoryWidget.h"
#include "SkillTreeWidget.h"
#include <QPainter>
#include <QDebug>
#include "videointro.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setFixedSize(950, 650);
    ui->setupUi(this);

    m_inventario = InventoryWidget::instance(this);
    m_inventario->hide();

    m_skillTree = new SkillTreeWidget(nullptr, this);
    m_skillTree->hide();

    connect(m_skillTree, &SkillTreeWidget::superJump1Unlocked,
            this, [this]()
            {
                if (m_player) {
                    m_player->setJumpMultiplier(2.0f);   // salto ×2
                    qDebug() << "[SkillTree] Super Salto I activado";
                }
            });

    pantallaInicio = new PantallaInicio(this);

    connect(pantallaInicio, &PantallaInicio::iniciarJuegoPresionado,
            this, [=]()
            {
                if (!menuOpciones) {
                    menuOpciones = new MenuOpciones(this);

                    connect(menuOpciones, &MenuOpciones::nuevaPartida,
                            this, [=]()
                            {
                                pantallaCarga = new PantallaCarga(this);

                                connect(pantallaCarga, &PantallaCarga::cargaCompletada,
                                        this, [=]()
                                        {
                                            m_player = new Jugador();
                                            m_skillTree->setPlayer(static_cast<Jugador*>(m_player));

                                            m_player->transform().setPosition(
                                                width() / 2.0, height() / 2.0);


                                            VideoIntro* primerVideo = new VideoIntro(this);
                                            primerVideo->setVideo("qrc:/resources/historia.mp4");

                                            mostrarPantalla(primerVideo);
                                            primerVideo->setFocus();

                                            connect(primerVideo, &VideoIntro::videoTerminado, this, [=]() {
                                                cargarNivel("Tutorial");
                                            });

                                        });
                                mostrarPantalla(pantallaCarga);
                            });
                }
                mostrarPantalla(menuOpciones);
            });

    mostrarPantalla(pantallaInicio);
}

MainWindow::~MainWindow()
{
    delete m_player;
    delete ui;
}

void MainWindow::mostrarPantalla(QWidget *pantalla)
{
    if (pantallaActual) pantallaActual->hide();
    pantallaActual = pantalla;
    setCentralWidget(pantallaActual);
    pantallaActual->show();
}
void MainWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(200, 200, 200));
}

void MainWindow::toggleInventory()
{
    if (!m_inventario) return;

    if (m_inventario->isVisible()) {
        m_inventario->hide();
    } else {
        const int w = m_inventario->width();
        const int h = m_inventario->height();
        const int x = (width()  - w) / 2;
        const int y = (height() - h) / 2;
        m_inventario->setGeometry(x, y, w, h);
        m_inventario->show();
        m_inventario->raise();
        m_inventario->setFocus();
    }
}

void MainWindow::toggleSkillTree()
{
    if (!m_skillTree) return;

    if (m_skillTree->isVisible())
        m_skillTree->hide();
    else {
        m_skillTree->show();
        m_skillTree->raise();
        m_skillTree->setFocus();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if (!ev->isAutoRepeat()) {
        if (ev->key() == Qt::Key_I) { toggleInventory();  return; }
        if (ev->key() == Qt::Key_U) { toggleSkillTree(); return; }
    }
    QMainWindow::keyPressEvent(ev);
}

void MainWindow::cargarNivel(const QString &nombre)
{
    if (nombre == "Tutorial") {
        auto *n = new TutorialScene(m_player, this);
        mostrarPantalla(n); n->setFocus();
    }
    else if (nombre == "TorreDeLaMarca") {
        auto *n = new niveltorredelamarca(m_player, this);
        mostrarPantalla(n); n->setFocus();
    }
    else if (nombre == "CiudadInversa") {
        auto *n = new ciudadinversa(m_player, this);
        mostrarPantalla(n); n->setFocus();
    }
    else if (nombre == "RaicesOlvidadas") {
        auto *n = new NivelRaicesOlvidadas(m_player, this);
        mostrarPantalla(n); n->setFocus();
    }
    else if (nombre == "MenteVacia") {
        auto *n = new mentevacia(m_player, this);
        mostrarPantalla(n); n->setFocus();
    }
    else if (nombre == "MaquinaDelOlvido") {
        auto* n = new maquina_olvido(m_player, this);
        mostrarPantalla(n);
        n->setFocus();
    }
}
