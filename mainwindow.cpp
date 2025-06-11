#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menuopciones.h"
#include "pantallainicio.h"
#include "pantallacarga.h"
#include "tutorialscene.h"
#include <QPainter>
#include <QDebug>
#include "nivelraicesolvidadas.h"
#include "videointro.h"
#include "jugador.h"
#include "niveltorredelamarca.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_player(nullptr),
    pantallaActual(nullptr),
    pantallaInicio(nullptr),
    menuOpciones(nullptr),
    pantallaCarga(nullptr),
    m_upPressed(false),
    m_downPressed(false),
    m_leftPressed(false),
    m_rightPressed(false),
    m_shiftPressed(false)
{
    setFixedSize(950, 650);
    ui->setupUi(this);
    pantallaInicio = new PantallaInicio(this);
    connect(pantallaInicio, &PantallaInicio::iniciarJuegoPresionado, this, [=]() {
        if (!menuOpciones) {
            menuOpciones = new MenuOpciones(this);
            connect(menuOpciones, &MenuOpciones::nuevaPartida, this, [=]() {
                pantallaCarga = new PantallaCarga(this);
                connect(pantallaCarga, &PantallaCarga::cargaCompletada, this, [=]() {
                    m_player = new Jugador();
                    m_player->sprite().loadFrames(SpriteState::Walking,":/resources/0_Blood_Demon_Walking_",24);
                    m_player->sprite().loadFrames(SpriteState::Idle,":/resources/0_Blood_Demon_Idle_",16);
                    m_player->sprite().loadFrames(SpriteState::IdleLeft,":/resources/0_Blood_Demon_IdleL_",16);
                    m_player->sprite().loadFrames(SpriteState::WalkingLeft,":/resources/0_Blood_Demon_WalkingL_",24);
                    m_player->sprite().loadFrames(SpriteState::Jump,":/resources/0_Blood_Demon_Jump Loop_",6);
                    m_player->sprite().generateMirroredFrames(SpriteState::Jump,     SpriteState::JumpLeft);
                    m_player->sprite().loadFrames(SpriteState::Running,":/resources/0_Blood_Demon_Running_",12);
                    m_player->sprite().generateMirroredFrames(SpriteState::Running,  SpriteState::RunningLeft);
                    m_player->sprite().loadFrames(SpriteState::Slashing,":/resources/0_Blood_Demon_Slashing_",12);
                    m_player->sprite().generateMirroredFrames(SpriteState::Slashing,  SpriteState::SlashingLeft);
                    m_player->sprite().loadFrames(SpriteState::Slidding,":/resources/0_Blood_Demon_Sliding_",6);
                    m_player->sprite().generateMirroredFrames(SpriteState::Slidding,  SpriteState::SliddingLeft);
                    m_player->sprite().loadFrames(SpriteState::dead,":/resources/0_Blood_Demon_Dying_",15);
                    m_player->sprite().generateMirroredFrames(SpriteState::dead,  SpriteState::deadleft);
                    m_player->sprite().setSize(128, 128);
                    float centerX = width()  / 2.0f;
                    float centerY = height() / 2.0f;
                    m_player->transform().setPosition(centerX, centerY);
                    m_player->sprite().setState(SpriteState::Idle);


                    // VideoIntro* primerVideo = new VideoIntro(this);
                    // primerVideo->setVideo("qrc:/resources/historia.mp4");

                    // connect(primerVideo, &VideoIntro::videoTerminado, this, [=]() {
                    //     VideoIntro* segundoVideo = new VideoIntro(this);
                    //     segundoVideo->setVideo("qrc:/resources/intro_silencion.mp4");

                        // connect(segundoVideo, &VideoIntro::videoTerminado, this, [=]() {
                         //   TutorialScene *tutorial = new TutorialScene(m_player, this);
                          //  mostrarPantalla(tutorial);
                           // tutorial->setFocus();


                   // NivelRaicesOlvidadas *n = new NivelRaicesOlvidadas(m_player, this);
                   // mostrarPantalla(n);
                   // n->setFocus();


                    niveltorredelamarca *n = new niveltorredelamarca(m_player, this);
                    mostrarPantalla(n);
                    n->setFocus();

                        });

                //         mostrarPantalla(segundoVideo);
                //     });

                //     mostrarPantalla(primerVideo);

                // });

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
    if (pantallaActual)
        pantallaActual->hide();
    pantallaActual = pantalla;
    setCentralWidget(pantallaActual);
    pantallaActual->show();
}
void MainWindow::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(200,200,200));
}
