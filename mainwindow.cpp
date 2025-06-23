#include "mainwindow.h"
#include "ui_mainwindow.h"

/* tus pantallas / niveles */
#include "pantallainicio.h"
#include "menuopciones.h"
#include "pantallacarga.h"
#include "tutorialscene.h"
#include "nivelraicesolvidadas.h"
#include "niveltorredelamarca.h"
#include "ciudadinversa.h"
#include "videointro.h"
#include "jugador.h"
#include "mentevacia.h"

/* inventario */
#include "InventoryWidget.h"

#include <QPainter>
#include <QDebug>

/* ========================================================= */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setFixedSize(950, 650);
    ui->setupUi(this);

    /* ➊ instancia única del inventario (hijo de MainWindow) */
    m_inventario = InventoryWidget::instance(this);

    /* ➋ tu flujo de pantallas original -------------------- */
    pantallaInicio = new PantallaInicio(this);

    connect(pantallaInicio, &PantallaInicio::iniciarJuegoPresionado, this,
            [=]()
            {
                if (!menuOpciones) {
                    menuOpciones = new MenuOpciones(this);

                    connect(menuOpciones, &MenuOpciones::nuevaPartida, this,
                            [=]()
                            {
                                /* pantalla de carga */
                                pantallaCarga = new PantallaCarga(this);

                                connect(pantallaCarga, &PantallaCarga::cargaCompletada,
                                        this, [=]()
                                        {
                                            /* ---------- creamos el jugador ---------- */
                                            m_player = new Jugador();
                                            auto& spr = m_player->sprite();
                                            spr.loadFrames(SpriteState::Walking, ":/resources/0_Blood_Demon_Walking_", 24);
                                            spr.loadFrames(SpriteState::Idle,    ":/resources/0_Blood_Demon_Idle_",    16);
                                            spr.loadFrames(SpriteState::IdleLeft,":/resources/0_Blood_Demon_IdleL_",   16);
                                            spr.loadFrames(SpriteState::WalkingLeft,":/resources/0_Blood_Demon_WalkingL_",24);
                                            spr.loadFrames(SpriteState::Jump,":/resources/0_Blood_Demon_Jump Loop_",6);
                                            spr.generateMirroredFrames(SpriteState::Jump, SpriteState::JumpLeft);
                                            spr.loadFrames(SpriteState::Running,":/resources/0_Blood_Demon_Running_",12);
                                            spr.generateMirroredFrames(SpriteState::Running, SpriteState::RunningLeft);
                                            spr.loadFrames(SpriteState::Slashing,":/resources/0_Blood_Demon_Slashing_",12);
                                            spr.generateMirroredFrames(SpriteState::Slashing, SpriteState::SlashingLeft);
                                            spr.loadFrames(SpriteState::Slidding,":/resources/0_Blood_Demon_Sliding_",6);
                                            spr.generateMirroredFrames(SpriteState::Slidding, SpriteState::SliddingLeft);
                                            spr.loadFrames(SpriteState::dead,":/resources/0_Blood_Demon_Dying_",15);
                                            spr.generateMirroredFrames(SpriteState::dead, SpriteState::deadleft);
                                            spr.loadFrames(SpriteState::throwing,":/resources/0_Blood_Demon_Throwing_",12);
                                            spr.generateMirroredFrames(SpriteState::throwing, SpriteState::throwingLeft);

                                            spr.setSize(128,128);
                                            spr.setState(SpriteState::Idle);
                                            m_player->transform().setPosition(width()/2.0, height()/2.0);




                                            /* -> primer nivel */
                                            cargarNivel("TorreDeLaMarca");
                                        });


                                mostrarPantalla(pantallaCarga);
                            });
                }
                mostrarPantalla(menuOpciones);
            });

}

/* ========================================================= */
MainWindow::~MainWindow()
{
    delete m_player;
    delete ui;
}

/* ========================================================= */
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
    p.fillRect(rect(), QColor(200,200,200));
}

/* ========================================================= */
void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    /* tecla global: inventario */
    if (ev->key() == Qt::Key_I) {
        m_inventario->toggleVisible();
        return;
    }
    QMainWindow::keyPressEvent(ev);
}

/* ========================================================= */
void MainWindow::cargarNivel(const QString &nombre)
{
    if (nombre == "Tutorial") {
        auto* n = new TutorialScene(m_player, this);
        mostrarPantalla(n);
        n->setFocus();
    }
    else if (nombre == "TorreDeLaMarca") {
        auto* n = new niveltorredelamarca(m_player, this);
        mostrarPantalla(n);
        n->setFocus();
    }
    else if (nombre == "CiudadInversa") {
        auto* n = new ciudadinversa(m_player, this);
        mostrarPantalla(n);
        n->setFocus();
    }
    else if (nombre == "RaicesOlvidadas") {
        auto* n = new NivelRaicesOlvidadas(m_player, this);
        mostrarPantalla(n);
        n->setFocus();
    }
    else if (nombre == "MenteVacia") {
        auto* n = new mentevacia(m_player, this);
        mostrarPantalla(n);
        n->setFocus();
    }else if (nombre == "MaquinaDelOlvido"){
    }
}
