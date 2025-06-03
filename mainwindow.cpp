#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menuopciones.h"
#include "pantallainicio.h"
#include "PantallaCarga.h"
#include <QPainter>
#include <QDebug>
#include "videointro.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    pantallaActual(nullptr),
    pantallaInicio(nullptr),
    menuOpciones(nullptr),
    pantallaCarga(nullptr),
    ui(new Ui::MainWindow),
    m_player(nullptr),
    m_timer(nullptr),
    m_upPressed(false),
    m_downPressed(false),
    m_leftPressed(false),
    m_rightPressed(false),
    m_shiftPressed(false)
{
    // Tamaño fijo para el área de juego
    setFixedSize(950, 650);
     ui->setupUi(this);

    connect(pantallaCarga, &PantallaCarga::cargaCompletada, this, [=]() {

        // 1) Creamos la entidad/jugador:
        m_player = new entidad();
        // Cargamos todas las animaciones del personaje
        m_player->sprite().loadFrames(SpriteState::Walking,":/resources/0_Blood_Demon_Walking_",24);
        m_player->sprite().loadFrames(SpriteState::Idle,":/resources/0_Blood_Demon_Idle_",16);
        m_player->sprite().loadFrames(SpriteState::IdleLeft, ":/resources/0_Blood_Demon_IdleL_",16);
        m_player->sprite().loadFrames(SpriteState::WalkingLeft,":/resources/0_Blood_Demon_WalkingL_",24);
        m_player->sprite().loadFrames(SpriteState::Jump, ":/resources/0_Blood_Demon_Jump Loop_",6);
        m_player->sprite().generateMirroredFrames(SpriteState::Jump,SpriteState::JumpLeft);
        m_player->sprite().loadFrames(SpriteState::Running, ":/resources/0_Blood_Demon_Running_",12);
        m_player->sprite().generateMirroredFrames(SpriteState::Running,SpriteState::RunningLeft);


         m_player->sprite().setSize(128, 128);
         float centerX = float(width())  / 2.0f;
         float centerY = float(height()) / 2.0f;

         m_player->transform().setPosition(centerX, centerY);
         m_player->sprite().setState(SpriteState::Idle);

         m_timer = new QTimer(this);
         connect(m_timer, &QTimer::timeout, this, &MainWindow::onGameLoop);

         m_timer->start(int(m_dt * 1000));

         setFocusPolicy(Qt::StrongFocus);
         setFocus();

         QWidget *temp = new QWidget(this);
         setCentralWidget(temp);

         temp->show();
         delete temp;
    });

    setFocusPolicy(Qt::NoFocus);

    pantallaInicio = new PantallaInicio(this);

    connect(pantallaInicio, &PantallaInicio::iniciarJuegoPresionado, this, [=]() {
        // Si se presiona el boton de iniciar Juego

        if (!menuOpciones) {
            menuOpciones = new MenuOpciones(this);
            connect(menuOpciones, &MenuOpciones::nuevaPartida, this, [=]() {
             // Se selecciono nueva partida

                // Creamos la pantalla de carga
                pantallaCarga = new PantallaCarga(this);

                // Ahora que pantallaCarga existe, conectamos su señal:
                connect(pantallaCarga, &PantallaCarga::cargaCompletada, this, [=]() {


                    // Creamos la pantalla de video y cargamos el video
                    // VideoIntro *video = new VideoIntro(this);
                    // mostrarPantalla(video);
                    // connect(video, &VideoIntro::videoTerminado, this, [=]() {

                    m_player = new entidad();
                    m_player->transform().setPosition(width()/2 - 32, height()/2 - 32);

                    m_timer = new QTimer(this);
                    connect(m_timer, &QTimer::timeout, this, &MainWindow::onGameLoop);
                    m_timer->start(int(m_dt * 1000));
                    setFocusPolicy(Qt::StrongFocus);
                    setFocus();

                    QWidget *temp = new QWidget(this);
                    setCentralWidget(temp);
                    temp->show();
                    delete temp;

                    // });

                });
                mostrarPantalla(pantallaCarga);
            });
        }
        mostrarPantalla(menuOpciones);
    });
    mostrarPantalla(pantallaInicio);
}
MainWindow::~MainWindow() {
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
    }
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

void MainWindow::onGameLoop()
{
    if (!m_player)
        return;
    processInput();
    m_player->actualizar(m_dt);
    update();
}

void MainWindow::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(200, 200, 200));

    if (m_player) {
        m_player->sprite().draw(painter);
        QPoint posSprite = m_player->sprite().getPosition();
        m_player->salud().dibujar(painter, posSprite);
    }
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
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

void MainWindow::processInput()
{
    if (!m_player) return;

    float baseSpeed = 160.0f;
    float moveSpeed = m_shiftPressed ? (baseSpeed * 2.0f) : baseSpeed;

    float vx = 0.0f;

    if (m_leftPressed && !m_rightPressed) {
        vx = -moveSpeed;
    }
    else if (m_rightPressed && !m_leftPressed) {
        vx = +moveSpeed;
    }
    else {
        vx = 0.0f;
    }

    m_player->fisica().setVelocity(vx, 0.0f);

    if (vx < 0.0f) {
        if (m_shiftPressed) {
            m_player->sprite().setState(SpriteState::RunningLeft);
        } else {
            m_player->sprite().setState(SpriteState::WalkingLeft);
        }
    }
    else if (vx > 0.0f) {
        if (m_shiftPressed) {
            m_player->sprite().setState(SpriteState::Running);
        } else {
            m_player->sprite().setState(SpriteState::Walking);
        }
    }
    else {

        if (m_player->facingleft()) {
            m_player->sprite().setState(SpriteState::IdleLeft);
        } else {
            m_player->sprite().setState(SpriteState::Idle);
        }
    }
}



