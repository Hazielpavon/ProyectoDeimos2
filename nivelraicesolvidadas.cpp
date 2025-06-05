#include "nivelraicesolvidadas.h"
#include <QPainter>
#include <QDebug>
#include <QGraphicsItem>

// Constantes locales (reemplazo de constantes.h)
constexpr float WINDOW_WIDTH     = 950.0f;
constexpr float WINDOW_HEIGHT    = 650.0f;
constexpr float PLAT_WIDTH       = 200.0f;
constexpr float PLAT_HEIGHT      = 20.0f;
constexpr float GRAVEDAD         = 980.0f;
constexpr float JUMP_VELOCITY    = 550.0f;
constexpr float FPS              = 60.0f;

NivelRaicesOlvidadas::NivelRaicesOlvidadas(entidad *jugadorPrincipal, QWidget *parent)
    : QWidget(parent),
    m_player(jugadorPrincipal),
    m_timer(new QTimer(this)),
    m_moverIzq(false),
    m_moverDer(false),
    m_shiftPresionado(false),
    m_saltoSolicitado(false),
    m_yaCaminó(false),
    m_yaSaltó(false),
    m_view(nullptr),
    m_scene(nullptr),
    m_fondoItem(nullptr),
    m_cartelItem(nullptr),
    m_jugadorItem(nullptr),
    m_plataformaItem(nullptr),
    m_sueloItem(nullptr),
    m_limiteSueloCentroY(0),
    m_dt(1.0f / FPS)

{

    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    setFixedSize(int(WINDOW_WIDTH), int(WINDOW_HEIGHT));

    // Zoom-out del fondo
    QPixmap pixFondoOriginal(":/resources/raices_olvidadas.png");
    if (pixFondoOriginal.isNull()) {
        qWarning() << "[TutorialScene] Error al cargar fondo";
    }
    QPixmap pixFondo = pixFondoOriginal.scaled(
        pixFondoOriginal.width() * 0.8,
        pixFondoOriginal.height() * 0.8,
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation
        );



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

    int sueloY = fondoH - 40;
    int topPlatY = sueloY - 160;

    // VISUAL
    QGraphicsRectItem *visualPlataforma = new QGraphicsRectItem(0, 0, PLAT_WIDTH + 120, 60);
    visualPlataforma->setBrush(QColor(80, 80, 80));
    visualPlataforma->setPen(Qt::NoPen);
    visualPlataforma->setZValue(0.5);
    visualPlataforma->setPos(300 - 60, topPlatY - 40); // X corregido
    m_scene->addItem(visualPlataforma);

    // COLISIÓN
    m_plataformaItem = new QGraphicsRectItem(0, 0, PLAT_WIDTH + 120, PLAT_HEIGHT);
    m_plataformaItem->setBrush(Qt::NoBrush);
    m_plataformaItem->setPen(Qt::NoPen);
    m_plataformaItem->setZValue(1);
    m_plataformaItem->setPos(300 - 60, topPlatY); // misma corrección
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

    connect(m_timer, &QTimer::timeout, this, &NivelRaicesOlvidadas::onFrame);
    m_timer->start(int(m_dt * 1000));

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    if (m_jugadorItem) {
        m_view->centerOn(m_jugadorItem);
    }
}
void NivelRaicesOlvidadas::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (!m_player) return;

    // Mouse click (ataque)
    if (m_player->isOnGround()) {
        if (m_player->getLastDirection() == SpriteState::WalkingLeft ||
            m_player->getLastDirection() == SpriteState::RunningLeft) {
            m_player->reproducirAnimacionTemporal(SpriteState::SlashingLeft, 0.6f);
        } else {
            m_player->reproducirAnimacionTemporal(SpriteState::Slashing, 0.6f);
        }
    }

}

// Captura de teclas
void NivelRaicesOlvidadas::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A: m_moverIzq = true; break;
    case Qt::Key_D: m_moverDer = true; break;
    case Qt::Key_Shift: m_shiftPresionado = true; break;
    case Qt::Key_Space: m_saltoSolicitado = true; break;
    // Tecla C (sliding)
    case Qt::Key_C:
        if (m_player && m_player->isOnGround()) {
            float vx = m_player->fisica().velocity().x();
            if (vx > 0.0f) {
                m_player->reproducirAnimacionTemporal(SpriteState::Slidding, 0.5f);
            } else if (vx < 0.0f) {
                m_player->reproducirAnimacionTemporal(SpriteState::SliddingLeft, 0.5f);
            }
        }
        break;

    default:
        QWidget::keyPressEvent(event);
    }
}


void NivelRaicesOlvidadas::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A: m_moverIzq = false; break;
    case Qt::Key_D: m_moverDer = false; break;
    case Qt::Key_Shift: m_shiftPresionado = false; break;
    default: QWidget::keyReleaseEvent(event);
    }
}

void NivelRaicesOlvidadas::onFrame()
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

    QRectF rectJugador(
        nuevaX - tamSpr.width() / 2.0f,
        nuevaY - tamSpr.height() / 2.0f,
        tamSpr.width(), tamSpr.height()
        );

    QRectF rectPlataforma = m_plataformaItem->sceneBoundingRect();
    QRectF sueloRect = m_sueloItem->rect();

    float vyPost = m_player->fisica().velocity().y();
    float pie = nuevaY + (tamSpr.height() / 2.0f);
    float cabeza = nuevaY - (tamSpr.height() / 2.0f);

    // Aterrizaje sobre plataforma
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
    // Golpe desde abajo a la plataforma
    else if (rectJugador.intersects(rectPlataforma) &&
             vyPost < 0.0f &&
             cabeza <= rectPlataforma.bottom() &&
             pie > rectPlataforma.bottom())
    {
        nuevaY = rectPlataforma.bottom() + tamSpr.height() / 2.0f;
        m_player->transform().setPosition(nuevaX, nuevaY);
        m_player->fisica().setVelocity(vx, 0.0f);
    }

    // Aterrizaje sobre suelo invisible
    if (vyPost >= 0.0f &&
        nuevaY >= sueloRect.top() - tamSpr.height() / 2.0f)
    {
        nuevaY = sueloRect.top() - tamSpr.height() / 2.0f;
        m_player->transform().setPosition(nuevaX, nuevaY);
        m_player->fisica().setVelocity(vx, 0.0f);
        m_player->setOnGround(true);
    }

    // Salto solo si está en el suelo
    if (m_saltoSolicitado && m_player->isOnGround()) {
        m_player->startJump();
        m_yaSaltó = true;
    }
    m_saltoSolicitado = false;

    QPixmap frameActual = m_player->sprite().currentFrame().scaled(
        tamSpr, Qt::KeepAspectRatio, Qt::SmoothTransformation
        );
    m_jugadorItem->setPixmap(frameActual);
    m_jugadorItem->setPos(
        nuevaX - frameActual.width() / 2,
        nuevaY - frameActual.height() / 2
        );

    if (m_view && m_jugadorItem) {
        m_view->centerOn(m_jugadorItem);
    }

    if (m_moverIzq || m_moverDer) m_yaCaminó = true;

    if (m_cartelItem && m_yaCaminó && m_yaSaltó) {
        m_scene->removeItem(m_cartelItem);
        delete m_cartelItem;
        m_cartelItem = nullptr;
    }
}

