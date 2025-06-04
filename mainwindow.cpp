#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QDebug>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include "menuopciones.h"
#include "pantallainicio.h"
#include "pantallacarga.h"
#include <QPainter>
#include <QDebug>
#include "jugador.h"
//#include "videointro.h"
#include "tutorialscene.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    pantallaInicio(nullptr),
    menuOpciones(nullptr),
    pantallaCarga(nullptr),
    pantallaActual(nullptr),
    m_player(nullptr),
    m_timer(nullptr),
    m_leftPressed(false),
    m_rightPressed(false),
    m_shiftPressed(false),
    m_cPressed(false)
{
    ui->setupUi(this);
    setFixedSize(950, 650);

    // 1) Creamos la pantalla de inicio (PantallaInicio hereda de QWidget y tiene Q_OBJECT)
    pantallaInicio = new PantallaInicio(this);

    // Conectamos su señal iniciarJuegoPresionado() al slot/lambda correspondiente
    connect(pantallaInicio, &PantallaInicio::iniciarJuegoPresionado, this, [=]() {


        // Abrir menú de opciones
        if (!menuOpciones) {
            menuOpciones = new MenuOpciones(this);
            connect(menuOpciones, &MenuOpciones::nuevaPartida, this, [=]() {
                // Crear pantalla de carga
                pantallaCarga = new PantallaCarga(this);

                // Conectamos la señal cargaCompletada() de PantallaCarga
                connect(pantallaCarga, &PantallaCarga::cargaCompletada, this, [=]() {
                    // Cuando termine la carga, arrancamos la entidad/jugador
                    m_player = new entidad();

                    // Re-cargamos animaciones en m_player
                    m_player->sprite().loadFrames(SpriteState::Slashing, ":/resources/0_Blood_Demon_Slashing_", 12);
                    m_player->sprite().generateMirroredFrames(SpriteState::Slashing, SpriteState::Slashingleft);

                    m_player->sprite().loadFrames(SpriteState::Walking, ":/resources/0_Blood_Demon_Walking_", 24);
                    m_player->sprite().loadFrames(SpriteState::WalkingLeft, ":/resources/0_Blood_Demon_WalkingL_", 24);
                    m_player->sprite().loadFrames(SpriteState::Idle, ":/resources/0_Blood_Demon_Idle_", 16);
                    m_player->sprite().loadFrames(SpriteState::IdleLeft, ":/resources/0_Blood_Demon_IdleL_", 16);

                    m_player->sprite().loadFrames(SpriteState::Jump, ":/resources/0_Blood_Demon_Jump Loop_", 6);
                    m_player->sprite().generateMirroredFrames(SpriteState::Jump, SpriteState::JumpLeft);

                    m_player->sprite().loadFrames(SpriteState::Running, ":/resources/0_Blood_Demon_Running_", 12);
                    m_player->sprite().generateMirroredFrames(SpriteState::Running, SpriteState::RunningLeft);

                    m_player->sprite().loadFrames(SpriteState::Sliding, ":/resources/0_Blood_Demon_Sliding_", 6);
                    m_player->sprite().generateMirroredFrames(SpriteState::Sliding, SpriteState::SlidingLeft);

                    m_player->sprite().setSize(128, 128);
                    float centerX = float(width()) / 2.0f;
                    float centerY = float(height()) / 2.0f;
                    m_player->transform().setPosition(centerX, centerY);
                    m_player->sprite().setState(SpriteState::Idle);

                    // Arrancamos el timer del game loop
                    m_timer = new QTimer(this);
                    connect(m_timer, &QTimer::timeout, this, &MainWindow::onGameLoop);
                    m_timer->start(int(m_dt * 1000));

                    setFocusPolicy(Qt::StrongFocus);
                    setFocus();

                    // Creamos la pantalla de video y cargamos el video
                    // VideoIntro *video = new VideoIntro(this);
                    // mostrarPantalla(video);
                    // connect(video, &VideoIntro::videoTerminado, this, [=]() {

                    TutorialScene* tutorial = new TutorialScene(this, m_player);
                    mostrarPantalla(tutorial);

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

    // Mostramos la pantalla de inicio por defecto
    mostrarPantalla(pantallaInicio);
}

MainWindow::~MainWindow()
{
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
    }
    delete m_player;
    delete ui;
}

void MainWindow::mostrarPantalla(QWidget *pant)
{
    if (pantallaActual)
        pantallaActual->hide();
    pantallaActual = pant;
    setCentralWidget(pantallaActual);
    pantallaActual->show();
}

void MainWindow::onGameLoop()
{
    if (!m_player) return;
    processInput();
    m_player->actualizar(m_dt);
    update();  // obligar a repintar
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
    case Qt::Key_C:
        m_cPressed = true;
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
    case Qt::Key_C:
        m_cPressed = false;
        break;
    default:
        QMainWindow::keyReleaseEvent(event);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (!m_player) {
        // Si aún no existe el jugador, lo dejamos pasar a QMainWindow
        QMainWindow::mousePressEvent(event);
        return;
    }

    // Si hacen clic izquierdo, disparamos el ataque:
    if (event->button() == Qt::LeftButton) {
        m_player->startAttack();
    } else {
        QMainWindow::mousePressEvent(event);
    }
}

void MainWindow::processInput()
{
    if (!m_player) return;

    // 1) Calculamos vx
    float baseSpeed = 160.0f;
    float moveSpeed = m_shiftPressed ? (baseSpeed * 2.0f) : baseSpeed;
    float vx = 0.0f;
    if (m_leftPressed && !m_rightPressed)      vx = -moveSpeed;
    else if (m_rightPressed && !m_leftPressed) vx = +moveSpeed;
    else                                       vx = 0.0f;

    // 2) Asignamos la velocidad
    m_player->fisica().setVelocity(vx, 0.0f);

    // 3) Si está saltando o en ataque, salimos sin cambiar animación:
    if (m_player->Isjumping() || m_player->IsAttacking()) {
        return;
    }

    // 4) Chequeo de “sliding” (tecla C + movimiento)
    if (m_cPressed && vx < 0.0f) {
        m_player->sprite().setState(SpriteState::SlidingLeft);
        return;
    }
    else if (m_cPressed && vx > 0.0f) {
        m_player->sprite().setState(SpriteState::Sliding);
        return;
    }

    // 5) Si no deslizamos, asignamos Walking/Running/Idle:
    if (vx < 0.0f) {
        if (m_shiftPressed)
            m_player->sprite().setState(SpriteState::RunningLeft);
        else
            m_player->sprite().setState(SpriteState::WalkingLeft);
    }
    else if (vx > 0.0f) {
        if (m_shiftPressed)
            m_player->sprite().setState(SpriteState::Running);
        else
            m_player->sprite().setState(SpriteState::Walking);
    }
    else {
        // vx == 0 → Idle
        if (m_player->facingleft())
            m_player->sprite().setState(SpriteState::IdleLeft);
        else
            m_player->sprite().setState(SpriteState::Idle);
    }
}

