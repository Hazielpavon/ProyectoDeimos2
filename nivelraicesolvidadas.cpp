#include "nivelraicesolvidadas.h"
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QImage>
#include <algorithm>
#include "mapawidget.h"

// Constants
static constexpr float WINDOW_W   = 950.0f;
static constexpr float WINDOW_H   = 650.0f;
static constexpr float FPS        = 60.0f;
static constexpr float PLAT_WIDTH = 200.0f;
static constexpr float PLAT_HEIGHT= 20.0f;

// Helper para recortar filas transparentes en la parte inferior
static QPixmap trimBottom(const QPixmap& pix) {
    QImage img = pix.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
    int maxY = -1;
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            if (qAlpha(img.pixel(x, y)) > 0) {
                maxY = std::max(maxY, y);
            }
        }
    }
    if (maxY >= 0 && maxY < img.height() - 1) {
        return pix.copy(0, 0, pix.width(), maxY + 1);
    }
    return pix;
}

NivelRaicesOlvidadas::NivelRaicesOlvidadas(entidad* jugador,
                                           MainWindow* mainWindow,
                                           QWidget* parent)
    : QWidget(parent)
    , m_player(jugador)
    , m_mainWindow(mainWindow)
    , m_timer(new QTimer(this))
    , m_view(nullptr)
    , m_scene(new QGraphicsScene(this))
    , m_colManager(new ObjetosYColisiones(m_scene, this))
    , m_playerItem(nullptr)
    , m_moveLeft(false)
    , m_moveRight(false)
    , m_run(false)
    , m_jumpRequested(false)
    , m_dt(1.0f / FPS)
    , m_repeatCount(1)
    , m_bgWidth(0)
    , m_bgHeight(0)
    , m_secondBgShown(false)
    ,m_mapaRegiones(nullptr)
    , m_currentRegion("Raices Olvidadas")
{
    setFixedSize(int(WINDOW_W), int(WINDOW_H));
    setFocusPolicy(Qt::StrongFocus);

    // Background
    QPixmap bgOrig(":/resources/raices_olvidadas.png");
    if (bgOrig.isNull()) qWarning() << "Error al cargar fondo";
    QPixmap bg = bgOrig.scaled(bgOrig.size() * 0.9,
                               Qt::KeepAspectRatioByExpanding,
                               Qt::SmoothTransformation);
    m_bgWidth = bg.width();
    m_bgHeight= bg.height();
    m_scene->setSceneRect(0, 0, m_bgWidth * m_repeatCount, m_bgHeight);
    for (int i = 0; i < m_repeatCount; ++i) {
        auto* itemBG = m_scene->addPixmap(bg);
        itemBG->setZValue(0);
        itemBG->setPos(i * m_bgWidth, 0);
    }

    // View
    m_view = new QGraphicsView(m_scene, this);
    m_view->setFixedSize(int(WINDOW_W), int(WINDOW_H));
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setAttribute(Qt::WA_TransparentForMouseEvents);

    // Plataformas
    float platX = 300.0f - (PLAT_WIDTH + 120.0f)/2.0f;
    float platY = (m_bgHeight - 40.0f) - 160.0f;
    m_colManager->addRect(QRectF(platX, platY,
                                 PLAT_WIDTH + 120.0f,
                                 PLAT_HEIGHT),
                          QColor(80,80,80), false);
    // Suelo (solo hitbox)
    m_colManager->addRect(QRectF(0.0f,
                                 m_bgHeight - 40.0f,
                                 float(m_bgWidth * m_repeatCount),
                                 40.0f),
                          Qt::NoBrush, true);

    // Sprite jugador: pie = footPos
    if (m_player) {
        QSize sz = m_player->sprite().getSize();
        QPixmap frame = m_player->sprite().currentFrame()
                            .scaled(sz, Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);
        // recortamos solo filas transparentes de abajo
        QPixmap spritePix = trimBottom(frame);

        float footX = platX + (PLAT_WIDTH + 120.0f)/2.0f;
        float footY = platY;
        m_player->transform().setPosition(footX, footY);
        m_player->setOnGround(true);

        m_playerItem = new QGraphicsPixmapItem;
        m_playerItem->setZValue(3);
        m_scene->addItem(m_playerItem);
        m_playerItem->setPixmap(spritePix);
        m_playerItem->setOffset(-spritePix.width()/2.0, -spritePix.height());
        m_playerItem->setPos(footX, footY);
    }

    m_mapaRegiones = new MapaWidget("Raices Olvidadas", this);  // ✅ Orden correcto
    m_mapaRegiones->setWindowModality(Qt::NonModal);  // ← para que no bloquee
    m_mapaRegiones->setFocusPolicy(Qt::NoFocus);      // ← que no robe el foco
    m_mapaRegiones->setAttribute(Qt::WA_ShowWithoutActivating);

    this->activateWindow();
    this->setFocus(Qt::OtherFocusReason);

    connect(m_mapaRegiones, &MapaWidget::mapaCerrado, this, [this]() {
        activateWindow();
        setFocus(Qt::OtherFocusReason);
    });

    connect(m_timer, &QTimer::timeout,
            this, &NivelRaicesOlvidadas::onFrame);
    m_timer->start(int(m_dt * 1000));
}

void NivelRaicesOlvidadas::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_A:    m_moveLeft      = true;  break;
    case Qt::Key_D:    m_moveRight     = true;  break;
    case Qt::Key_Shift:m_run           = true;  break;
    case Qt::Key_Space:m_jumpRequested = true;  break;
    case Qt::Key_C:
        if (m_player && m_player->isOnGround()) {
            float vx = m_player->fisica().velocity().x();
            auto state = vx > 0.0f ? SpriteState::Slidding
                                   : SpriteState::SliddingLeft;
            m_player->reproducirAnimacionTemporal(state,0.5f);
        }
        break;
    case Qt::Key_M:
        if (m_mapaRegiones) {
            if (!m_mapaRegiones->isVisible()) {
                m_mapaRegiones->show();
            } else {
                m_mapaRegiones->close();
            }
        }
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void NivelRaicesOlvidadas::keyReleaseEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_A:    m_moveLeft  = false; break;
    case Qt::Key_D:    m_moveRight = false; break;
    case Qt::Key_Shift:m_run        = false; break;
    default:
        QWidget::keyReleaseEvent(event);
    }
}

void NivelRaicesOlvidadas::mousePressEvent(QMouseEvent* event)
{
    if (!m_player || !m_player->isOnGround()) return;
    auto dir = m_player->getLastDirection();
    auto state = (dir==SpriteState::WalkingLeft||dir==SpriteState::RunningLeft)
                     ? SpriteState::SlashingLeft
                     : SpriteState::Slashing;
    m_player->reproducirAnimacionTemporal(state,0.6f);
}

void NivelRaicesOlvidadas::onFrame()
{
    if (!m_player) return;

    // Salto
    if (m_jumpRequested && m_player->isOnGround()) {
        constexpr float JUMP_SPEED = 500.0f;
        float vx = m_player->fisica().velocity().x();
        m_player->fisica().setVelocity(vx, -JUMP_SPEED);
        m_player->setOnGround(false);
    }

    // Movimiento horiz.
    float vx = 0;
    if (m_moveLeft)  vx = -160.0f;
    if (m_moveRight) vx =  160.0f;
    if (m_run && vx!=0) vx*=2;
    m_player->fisica().setVelocity(vx,
                                   m_player->fisica().velocity().y());

    // Física + colisiones
    m_player->actualizar(m_dt);
    m_colManager->resolveCollisions(m_player, m_dt);

    // Dibujo sprite + cámara
    QPointF footPos = m_player->transform().getPosition();
    QSize   sz      = m_player->sprite().getSize();
    QPixmap frame   = m_player->sprite().currentFrame()
                        .scaled(sz, Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
    QPixmap spritePix = trimBottom(frame);
    m_playerItem->setPixmap(spritePix);
    m_playerItem->setOffset(-spritePix.width()/2.0, -spritePix.height());
    m_playerItem->setPos(footPos);

    m_view->centerOn(footPos);
    m_jumpRequested = false;
}
