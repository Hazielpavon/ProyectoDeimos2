#include "tutorialscene.h"
#include <QPainter>
#include <QDebug>
#include <QGraphicsItem>
#include "nivelraicesolvidadas.h"
#include "mainwindow.h"
#include "mapawindow.h"
// Constantes locales (reemplazo de constantes.h)
#include "tutorialscene.h"
#include <QPainter>
#include <QDebug>


// Constantes locales (mismas que en NivelRaicesOlvidadas)
constexpr float WINDOW_WIDTH     = 950.0f;
constexpr float WINDOW_HEIGHT    = 650.0f;
constexpr float PLAT_WIDTH       = 200.0f;
constexpr float PLAT_HEIGHT      = 20.0f;
constexpr float FPS              = 60.0f;

TutorialScene::TutorialScene(entidad *jugadorPrincipal, MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent),
    m_player(jugadorPrincipal),
    m_timer(new QTimer(this)),
    m_moverIzq(false),
    m_moverDer(false),
    m_shiftPresionado(false),
    m_saltoSolicitado(false),
    m_yaCaminó(false),
    m_yaSaltó(false),
    m_yaCorrió(false),
    m_yaHizoDash(false),
    m_yaGolpeó(false),
    m_view(nullptr),
    m_scene(nullptr),
    m_jugadorItem(nullptr),
    m_plataformaItem(nullptr),
    m_sueloItem(nullptr),
    m_limiteSueloCentroY(0),
    m_dt(1.0f / FPS),
    m_instruccionCaminarItem(nullptr),
    m_instruccionSaltarItem(nullptr),
    m_instruccionCorrerItem(nullptr),
    m_instruccionDashItem(nullptr),
    m_instruccionGolpearItem(nullptr),
    m_mostrarSaltarPendiente(false),
    m_tiempoParaMostrarSaltar(2.0f),
    m_saltoYaMostrado(false),
    m_mostrarCorrerPendiente(false),
    m_tiempoParaMostrarCorrer(0.0f),
    m_correrYaMostrado(false),
    m_mostrarDashPendiente(false),
    m_tiempoParaMostrarDash(0.0f),
    m_dashYaMostrado(false),
    m_mostrarGolpearPendiente(false),
    m_tiempoParaMostrarGolpear(0.0f),
    m_golpearYaMostrado(false),
    m_mainWindow(mainWindow),
    m_mapaRegiones(nullptr),
    m_regionActual("Templo del Silencio"),
    m_instruccionMapaItem (nullptr),
    m_mostrarMapaPendiente(false),
    m_tiempoParaMostrarMapa (0.0f),
    m_mapaYaMostrado (false),
    m_yaAbrioMapa (false)

{
    setFixedSize(int(WINDOW_WIDTH), int(WINDOW_HEIGHT));
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    QPixmap pixFondoOriginal(":/resources/templo_silencio.png");
    if (pixFondoOriginal.isNull()) {
        qWarning() << "[TutorialScene] Error al cargar :/resources/templo_silencio.png";
    }
    QPixmap pixFondo = pixFondoOriginal.scaled(pixFondoOriginal.width() * 0.9f,pixFondoOriginal.height() * 0.9f, Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation);

    int fondoW = pixFondo.width();
    int fondoH = pixFondo.height();
    const int repeticiones = 3;

    m_scene = new QGraphicsScene(0, 0, fondoW * repeticiones, fondoH, this);
    for (int i = 0; i < repeticiones; ++i) {
        QGraphicsPixmapItem *itemFondo = new QGraphicsPixmapItem(pixFondo);
        itemFondo->setZValue(0);
        itemFondo->setPos(i * fondoW, 0);
        m_scene->addItem(itemFondo);
    }

    int sueloY   = fondoH - 40;
    int topPlatY = sueloY - 160;

    QGraphicsRectItem *visualPlataforma = new QGraphicsRectItem(0, 0, PLAT_WIDTH + 120, 60);
    visualPlataforma->setBrush(QColor(80, 80, 80));
    visualPlataforma->setPen(Qt::NoPen);
    visualPlataforma->setZValue(0.5);
    visualPlataforma->setPos(300 - 60, topPlatY - 40);
    m_scene->addItem(visualPlataforma);

    m_plataformaItem = new QGraphicsRectItem(0, 0, PLAT_WIDTH + 120, PLAT_HEIGHT);
    m_plataformaItem->setBrush(Qt::NoBrush);
    m_plataformaItem->setPen(Qt::NoPen);
    m_plataformaItem->setZValue(1);
    m_plataformaItem->setPos(300 - 60, topPlatY);
    m_scene->addItem(m_plataformaItem);

    m_sueloItem = new QGraphicsRectItem(0, sueloY, fondoW * repeticiones, 40);
    m_sueloItem->setPen(Qt::NoPen);
    m_sueloItem->setBrush(Qt::NoBrush);
    m_sueloItem->setZValue(2);
    m_scene->addItem(m_sueloItem);

    if (m_player) {
        QSize tamSprite = m_player->sprite().getSize();
        float yCentro = float(sueloY) - float(tamSprite.height()) + (tamSprite.height() / 2.0f);
        float xCentro = (300 - 60) + (PLAT_WIDTH + 120) / 2.0f;

        m_player->transform().setPosition(xCentro, yCentro);
        m_player->setOnGround(true);
        m_limiteSueloCentroY = yCentro;
    }

    if (m_player) {
        QPointF posIni = m_player->transform().getPosition();
        QSize tamSpr = m_player->sprite().getSize();
        m_player->sprite().setPosition(
            int(posIni.x() - tamSpr.width() / 2),
            int(posIni.y() - tamSpr.height() / 2)
            );
    }

    m_jugadorItem = new QGraphicsPixmapItem();
    m_jugadorItem->setZValue(3);
    m_scene->addItem(m_jugadorItem);
    if (m_player) {
        QPixmap frameIni = m_player->sprite().currentFrame();
        QSize tamSpr = m_player->sprite().getSize();
        if (!frameIni.isNull()) {
            QPixmap escala = frameIni.scaled(tamSpr, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            m_jugadorItem->setPixmap(escala);
            m_jugadorItem->setPos(
                m_player->transform().getPosition().x() - escala.width() / 2,
                m_player->transform().getPosition().y() - escala.height() / 2
                );
        }
    }

    m_view = new QGraphicsView(m_scene, this);
    m_view->setFixedSize(int(WINDOW_WIDTH), int(WINDOW_HEIGHT));
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->move(0, 0);
    m_view->setFocusPolicy(Qt::NoFocus);
    m_view->setAttribute(Qt::WA_TransparentForMouseEvents);

    if (m_jugadorItem) {
        m_view->centerOn(m_jugadorItem);
    }

    QPixmap pixCaminar(":/resources/caminar.png");
    if (!pixCaminar.isNull()) {
        m_instruccionCaminarItem = new QGraphicsPixmapItem(pixCaminar);
        m_instruccionCaminarItem->setZValue(5);
        m_scene->addItem(m_instruccionCaminarItem);
    }

    m_mapaRegiones = new QLabel(this);
    QPixmap mapaPix(":/resources/Regiones.png");
    if (mapaPix.isNull()) {
        qWarning() << "[TutorialScene] No se pudo cargar :/resources/Regiones.png";
    }

    QPixmap mapaEscalado = mapaPix.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_mapaRegiones->setPixmap(mapaEscalado);

    constexpr int MAP_WIDTH  = 400;
    constexpr int MAP_HEIGHT = 300;
    int xCentroMapa = (int(WINDOW_WIDTH)  - MAP_WIDTH)  / 2;
    int yCentroMapa = (int(WINDOW_HEIGHT) - MAP_HEIGHT) / 2;
    m_mapaRegiones->setGeometry(xCentroMapa, yCentroMapa, MAP_WIDTH, MAP_HEIGHT);

    m_mapaRegiones->setStyleSheet("background-color: rgba(0, 0, 0, 180);""border: 2px solid white;");

    m_mapaRegiones->setVisible(false);
    m_mapaRegiones->raise();


    connect(m_timer, &QTimer::timeout, this, &TutorialScene::onFrame);
    m_timer->start(int(m_dt * 1000));
}

void TutorialScene::mousePressEvent(QMouseEvent *event)
{

    if (!m_player) return;
    if (m_player->isOnGround()) {
        if (m_player->getLastDirection() == SpriteState::WalkingLeft ||
            m_player->getLastDirection() == SpriteState::RunningLeft) {
            m_player->reproducirAnimacionTemporal(SpriteState::SlashingLeft, 0.6f);
        } else {
            m_player->reproducirAnimacionTemporal(SpriteState::Slashing, 0.6f);
        }
        m_yaGolpeó = true;
        if (m_instruccionGolpearItem) {
            m_scene->removeItem(m_instruccionGolpearItem);
            delete m_instruccionGolpearItem;
            m_instruccionGolpearItem = nullptr;
        }
    }


    QWidget::mousePressEvent(event);
    if (m_mapaRegiones && m_mapaRegiones->isVisible()) {
        QPoint posMapa = m_mapaRegiones->mapFromParent(event->pos());
        QRect zonaTemplo(30, 30, 100, 80);
        QRect zonaRaices(40, 170, 100, 80);

        if (zonaTemplo.contains(posMapa)) {
            if (m_regionActual == "Raices Olvidadas") {
                m_regionActual = "Templo del Silencio";
                m_mapaRegiones->hide();

                TutorialScene *tutorial = new TutorialScene(m_player, m_mainWindow);
                m_mainWindow->mostrarPantalla(tutorial);
                tutorial->setFocus();
                return;
            }
        }
        else if (zonaRaices.contains(posMapa)) {
            if (m_regionActual == "Templo del Silencio") {
                m_regionActual = "Raices Olvidadas";
                m_mapaRegiones->hide();

                NivelRaicesOlvidadas *escenaRaices = new NivelRaicesOlvidadas(m_player, m_mainWindow);
                m_mainWindow->mostrarPantalla(escenaRaices);
                escenaRaices->setFocus();
                return;
            }
        }

}

}

void TutorialScene::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A:
        m_moverIzq = true;
        break;
    case Qt::Key_D:
        m_moverDer = true;
        break;
    case Qt::Key_Shift:
        m_shiftPresionado = true;
        break;
    case Qt::Key_Space:
        m_saltoSolicitado = true;
        break;
    case Qt::Key_Tab:
        if (m_mapaRegiones) {
            if (!m_mapaRegiones->isVisible()) {
                m_mapaRegiones->show();
            } else {
                m_mapaRegiones->close();
            }
            m_mapaRegiones->raise();
        }

        // Mostrar instrucción solo la primera vez
        if (!m_yaAbrioMapa) {
            m_yaAbrioMapa = true;

            if (m_instruccionMapaItem) {
                m_scene->removeItem(m_instruccionMapaItem);
                delete m_instruccionMapaItem;
                m_instruccionMapaItem = nullptr;
            }
        }
        break;
    case Qt::Key_C:
        if (m_player && m_player->isOnGround()) {
            float vx = m_player->fisica().velocity().x();
            if (vx > 0.0f) {
                m_player->reproducirAnimacionTemporal(SpriteState::Slidding, 0.5f);
            } else if (vx < 0.0f) {
                m_player->reproducirAnimacionTemporal(SpriteState::SliddingLeft, 0.5f);
            }
            m_yaHizoDash = true;
        }
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void TutorialScene::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A:
        m_moverIzq = false;
        break;
    case Qt::Key_D:
        m_moverDer = false;
        break;
    case Qt::Key_Shift:
        m_shiftPresionado = false;
        break;
    default:
        QWidget::keyReleaseEvent(event);
    }
}

void TutorialScene::onFrame()
{
    if (!m_player) return;

    float velocidadBase = 160.0f;
    float vx = 0.0f;
    if (m_moverIzq) vx = -velocidadBase;
    else if (m_moverDer) vx = +velocidadBase;
    if (m_shiftPresionado && vx != 0.0f) vx *= 2.0f;

    QPointF posActual = m_player->transform().getPosition();
    float vy = m_player->fisica().velocity().y();
    QSize tamSpr = m_player->sprite().getSize();

    m_player->fisica().setVelocity(vx, vy);
    m_player->actualizar(m_dt);

    float nuevaX = posActual.x() + vx * m_dt;
    float nuevaY = m_player->transform().getPosition().y();
    m_player->transform().setPosition(nuevaX, nuevaY);

    QRectF rectJugador(nuevaX - tamSpr.width() / 2.0f, nuevaY - tamSpr.height() / 2.0f,
                       tamSpr.width(), tamSpr.height());
    QRectF rectPlataforma = m_plataformaItem->sceneBoundingRect();
    QRectF sueloRect       = m_sueloItem->rect();

    float vyPost = m_player->fisica().velocity().y();
    float pie    = nuevaY + (tamSpr.height() / 2.0f);
    float cabeza = nuevaY - (tamSpr.height() / 2.0f);

    if (rectJugador.intersects(rectPlataforma) &&
        vyPost >= 0.0f &&
        pie >= rectPlataforma.top() &&
        cabeza < rectPlataforma.top())
    {
        nuevaY = rectPlataforma.top() - tamSpr.height() / 2.0f;
        m_player->transform().setPosition(nuevaX, nuevaY);
        m_player->fisica().setVelocity(vx, 0.0f);
        m_player->setOnGround(true);
    }

    else if (rectJugador.intersects(rectPlataforma) &&
             vyPost < 0.0f &&
             cabeza <= rectPlataforma.bottom() &&
             pie > rectPlataforma.bottom())
    {
        nuevaY = rectPlataforma.bottom() + tamSpr.height() / 2.0f;
        m_player->transform().setPosition(nuevaX, nuevaY);
        m_player->fisica().setVelocity(vx, 0.0f);
    }

    if (vyPost >= 0.0f && nuevaY >= sueloRect.top() - tamSpr.height() / 2.0f) {
        nuevaY = sueloRect.top() - tamSpr.height() / 2.0f;
        m_player->transform().setPosition(nuevaX, nuevaY);
        m_player->fisica().setVelocity(vx, 0.0f);
        m_player->setOnGround(true);
    }

    if (m_saltoSolicitado && m_player->isOnGround()) {
        m_player->startJump();
        m_yaSaltó = true;
        if (m_instruccionSaltarItem) {
            m_scene->removeItem(m_instruccionSaltarItem);
            delete m_instruccionSaltarItem;
            m_instruccionSaltarItem = nullptr;
        }
    }
    m_saltoSolicitado = false;

    QPixmap frameActual = m_player->sprite().currentFrame().scaled(
        tamSpr, Qt::KeepAspectRatio, Qt::SmoothTransformation
        );
    m_jugadorItem->setPixmap(frameActual);
    m_jugadorItem->setPos(nuevaX - frameActual.width() / 2, nuevaY - frameActual.height() / 2);

    if (m_instruccionCaminarItem) {
        m_instruccionCaminarItem->setPos(nuevaX - m_instruccionCaminarItem->pixmap().width() / 2, nuevaY - tamSpr.height() - 120);
    }
    if (m_instruccionSaltarItem) {
        m_instruccionSaltarItem->setPos(nuevaX - m_instruccionSaltarItem->pixmap().width() / 2, nuevaY - tamSpr.height() - 120);
    }
    if (m_instruccionCorrerItem) {
        m_instruccionCorrerItem->setPos(nuevaX - m_instruccionCorrerItem->pixmap().width() / 2,nuevaY - tamSpr.height() - 120);
    }
    if (m_instruccionDashItem) {
        m_instruccionDashItem->setPos(nuevaX - m_instruccionDashItem->pixmap().width() / 2, nuevaY - tamSpr.height() - 120);
    }
    if (m_instruccionGolpearItem) {
        m_instruccionGolpearItem->setPos(nuevaX - m_instruccionGolpearItem->pixmap().width() / 2, nuevaY - tamSpr.height() - 120);
    }
    if (m_instruccionMapaItem) {
        m_instruccionMapaItem->setPos(
            nuevaX - m_instruccionMapaItem->pixmap().width() / 2,
            nuevaY - tamSpr.height() - 120
            );
    }

    if (m_view && m_jugadorItem) {
        m_view->centerOn(m_jugadorItem);
    }

    if (m_moverIzq || m_moverDer) m_yaCaminó = true;
    if (m_yaCaminó && m_instruccionCaminarItem) {
        m_scene->removeItem(m_instruccionCaminarItem);
        delete m_instruccionCaminarItem;
        m_instruccionCaminarItem = nullptr;
    }

    if (m_yaCaminó && !m_yaSaltó && !m_mostrarSaltarPendiente &&!m_instruccionCaminarItem && !m_instruccionSaltarItem && !m_instruccionCorrerItem)
    {
        m_mostrarSaltarPendiente = true;
        m_tiempoParaMostrarSaltar = 2.0f;
    }
    if (m_mostrarSaltarPendiente) {
        m_tiempoParaMostrarSaltar -= m_dt;
        if (m_tiempoParaMostrarSaltar <= 0.0f) {
            QPixmap pixSaltar(":/resources/saltar.png");
            if (!pixSaltar.isNull()) {
                m_instruccionSaltarItem = new QGraphicsPixmapItem(pixSaltar);
                m_instruccionSaltarItem->setZValue(5);
                m_scene->addItem(m_instruccionSaltarItem);
                m_saltoYaMostrado = true;
            }
            m_mostrarSaltarPendiente = false;
        }
    }

    if (m_yaSaltó && m_saltoYaMostrado && !m_yaCorrió &&!m_mostrarCorrerPendiente &&!m_instruccionCaminarItem && !m_instruccionSaltarItem && !m_instruccionCorrerItem)
    {
        m_mostrarCorrerPendiente = true;
        m_tiempoParaMostrarCorrer = 2.0f;
    }
    if (m_mostrarCorrerPendiente) {
        m_tiempoParaMostrarCorrer -= m_dt;
        if (m_tiempoParaMostrarCorrer <= 0.0f) {
            QPixmap pixCorrer(":/resources/correr.png");
            if (!pixCorrer.isNull()) {
                m_instruccionCorrerItem = new QGraphicsPixmapItem(pixCorrer);
                m_instruccionCorrerItem->setZValue(5);
                m_scene->addItem(m_instruccionCorrerItem);
                m_correrYaMostrado = true;
            }
            m_mostrarCorrerPendiente = false;
        }
    }

    if ((m_moverIzq || m_moverDer) && m_shiftPresionado) {
        m_yaCorrió = true;
    }
    if (m_yaCorrió && m_instruccionCorrerItem) {
        m_scene->removeItem(m_instruccionCorrerItem);
        delete m_instruccionCorrerItem;
        m_instruccionCorrerItem = nullptr;
    }
    if (m_yaCorrió && m_correrYaMostrado && !m_yaHizoDash &&!m_mostrarDashPendiente && !m_instruccionCaminarItem && !m_instruccionSaltarItem && !m_instruccionCorrerItem && !m_instruccionDashItem)
    {
        m_mostrarDashPendiente = true;
        m_tiempoParaMostrarDash = 2.0f;
    }
    if (m_mostrarDashPendiente) {
        m_tiempoParaMostrarDash -= m_dt;
        if (m_tiempoParaMostrarDash <= 0.0f) {
            QPixmap pixDash(":/resources/dash.png");
            if (!pixDash.isNull()) {
                m_instruccionDashItem = new QGraphicsPixmapItem(pixDash);
                m_instruccionDashItem->setZValue(5);
                m_scene->addItem(m_instruccionDashItem);
                m_dashYaMostrado = true;
            }
            m_mostrarDashPendiente = false;
        }
    }

    if (m_yaHizoDash && m_instruccionDashItem) {
        m_scene->removeItem(m_instruccionDashItem);
        delete m_instruccionDashItem;
        m_instruccionDashItem = nullptr;
    }
    if (m_yaHizoDash && m_dashYaMostrado && !m_yaGolpeó &&!m_mostrarGolpearPendiente &&!m_instruccionCaminarItem && !m_instruccionSaltarItem && !m_instruccionCorrerItem && !m_instruccionDashItem && !m_instruccionGolpearItem)
    {
        m_mostrarGolpearPendiente = true;
        m_tiempoParaMostrarGolpear = 2.0f;
    }
    if (m_mostrarGolpearPendiente) {
        m_tiempoParaMostrarGolpear -= m_dt;
        if (m_tiempoParaMostrarGolpear <= 0.0f) {
            QPixmap pixGolpear(":/resources/golpear.png");
            if (!pixGolpear.isNull()) {
                m_instruccionGolpearItem = new QGraphicsPixmapItem(pixGolpear);
                m_instruccionGolpearItem->setZValue(5);
                m_scene->addItem(m_instruccionGolpearItem);
                m_golpearYaMostrado = true;
            }
            m_mostrarGolpearPendiente = false;
        }
    }

    if (m_yaGolpeó && m_instruccionGolpearItem) {
        m_scene->removeItem(m_instruccionGolpearItem);
        delete m_instruccionGolpearItem;
        m_instruccionGolpearItem = nullptr;
    }

    if (m_yaGolpeó && m_golpearYaMostrado && !m_yaAbrioMapa &&
        !m_mostrarMapaPendiente &&
        m_instruccionCaminarItem == nullptr &&
        m_instruccionSaltarItem == nullptr &&
        m_instruccionCorrerItem == nullptr &&
        m_instruccionDashItem == nullptr &&
        m_instruccionGolpearItem == nullptr &&
        m_instruccionMapaItem == nullptr)
    {
        m_mostrarMapaPendiente = true;
        m_tiempoParaMostrarMapa = 2.0f;
    }

if (m_mostrarMapaPendiente) {
    m_tiempoParaMostrarMapa -= m_dt;
    if (m_tiempoParaMostrarMapa <= 0.0f) {
        QPixmap pixMapa(":/resources/mapa.png");
        if (!pixMapa.isNull()) {
            m_instruccionMapaItem = new QGraphicsPixmapItem(pixMapa);
            m_instruccionMapaItem->setZValue(5);
            m_scene->addItem(m_instruccionMapaItem);
        }
        m_mapaYaMostrado = true;
        m_mostrarMapaPendiente = false;
    }
}
}
