#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menuopciones.h"
#include "pantallainicio.h"
#include "pantallacarga.h"
#include "tutorialscene.h"
#include <QPainter>
#include <QDebug>

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

    // 1) Pantalla de inicio
    pantallaInicio = new PantallaInicio(this);

    // 2) Cuando el usuario pulsa “Iniciar Juego” → muestro menúOpciones
    connect(pantallaInicio, &PantallaInicio::iniciarJuegoPresionado, this, [=]() {
        if (!menuOpciones) {
            menuOpciones = new MenuOpciones(this);
            connect(menuOpciones, &MenuOpciones::nuevaPartida, this, [=]() {
                // 3) Creamos pantallaCarga
                pantallaCarga = new PantallaCarga(this);
                connect(pantallaCarga, &PantallaCarga::cargaCompletada, this, [=]() {
                    // 3.1) Instanciamos m_player y cargamos frames:
                    // Dentro de tu connect de pantallaCarga->cargaCompletada:

                    // … dentro del connect de pantallaCarga->cargaCompletada:

                    m_player = new entidad();
                    // 1) Cargar TODOS los frames del sprite (igual que siempre):
                    m_player->sprite().loadFrames(SpriteState::Walking,      ":/resources/0_Blood_Demon_Walking_",   24);
                    m_player->sprite().loadFrames(SpriteState::Idle,         ":/resources/0_Blood_Demon_Idle_",      16);
                    m_player->sprite().loadFrames(SpriteState::IdleLeft,     ":/resources/0_Blood_Demon_IdleL_",     16);
                    m_player->sprite().loadFrames(SpriteState::WalkingLeft,  ":/resources/0_Blood_Demon_WalkingL_",  24);
                    m_player->sprite().loadFrames(SpriteState::Jump,         ":/resources/0_Blood_Demon_Jump Loop_",  6);
                    m_player->sprite().generateMirroredFrames(SpriteState::Jump,     SpriteState::JumpLeft);
                    m_player->sprite().loadFrames(SpriteState::Running,      ":/resources/0_Blood_Demon_Running_",   12);
                    m_player->sprite().generateMirroredFrames(SpriteState::Running,  SpriteState::RunningLeft);
                    m_player->sprite().setSize(128, 128);

                    // 2) Lo colocamos provisionalmente en el centro (TutorialScene lo reubicará en el suelo):
                    float centerX = width()  / 2.0f;
                    float centerY = height() / 2.0f;
                    m_player->transform().setPosition(centerX, centerY);
                    m_player->sprite().setState(SpriteState::Idle);

                    // 3) Creamos la NUEVA TutorialScene (la que dibuja con QPainter)
                    TutorialScene *tutorial = new TutorialScene(m_player, this);
                    mostrarPantalla(tutorial);

                    // 4) ¡Clave! Darle foco inmediatamente para que reciba teclas:
                    tutorial->setFocus();

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
    if (pantallaActual)
        pantallaActual->hide();
    pantallaActual = pantalla;
    setCentralWidget(pantallaActual);
    pantallaActual->show();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Durante el tutorial, TutorialScene se queda con el foco y maneja teclas,
    // así que aquí solo procesamos si m_player existe y estamos dibujando fuera del tutorial:
    if (!m_player) {
        QMainWindow::keyPressEvent(event);
        return;
    }

    switch (event->key()) {
    case Qt::Key_A:
    case Qt::Key_Left:
        m_leftPressed = true;
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        m_rightPressed = true;
        break;
    case Qt::Key_Shift:
        m_shiftPressed = true;
        break;
    case Qt::Key_Space:
        m_player->startJump();
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (!m_player) {
        QMainWindow::keyReleaseEvent(event);
        return;
    }

    switch (event->key()) {
    case Qt::Key_A:
    case Qt::Key_Left:
        m_leftPressed = false;
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        m_rightPressed = false;
        break;
    case Qt::Key_Shift:
        m_shiftPressed = false;
        break;
    default:
        QMainWindow::keyReleaseEvent(event);
    }
}

// Como no usamos onGameLoop durante el tutorial, podéis dejarlo vacío o eliminarlo:
void MainWindow::onGameLoop()
{
    // No hace nada mientras estemos en TutorialScene,
    // porque la propia TutorialScene maneja su timer interno.
}

void MainWindow::paintEvent(QPaintEvent * /*event*/)
{
    // (Opcional) Si querías pintar algo extra en MainWindow, lo dejas aquí.
    // En modo “TutorialScene” todo se dibuja en el QGraphicsView interno.
    QPainter painter(this);
    painter.fillRect(rect(), QColor(200,200,200));
}
