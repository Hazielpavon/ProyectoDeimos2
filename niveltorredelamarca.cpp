#include "niveltorredelamarca.h"
#include "mapawidget.h"
#include "ObjetosYColisiones.h"
#include <QPen>
#include <QFont>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QImage>
#include <algorithm>
#include "jugador.h"
// Constantes de ventana, FPS y plataforma
static constexpr float WINDOW_W    = 950.0f;
static constexpr float WINDOW_H    = 650.0f;
static constexpr float FPS         = 60.0f;
static constexpr float PLAT_WIDTH  = 200.0f;
static constexpr float PLAT_HEIGHT = 20.0f;

// Helper para recortar filas transparentes en la parte inferior
static QPixmap trimBottom(const QPixmap& pix) {
    QImage img = pix.toImage()
    .convertToFormat(QImage::Format_ARGB32_Premultiplied);
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

niveltorredelamarca::niveltorredelamarca(entidad* jugador,
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
    , m_mapaRegiones(nullptr)
    , m_currentRegion("Raices Olvidadas")
{
    setFixedSize(int(WINDOW_W), int(WINDOW_H));
    setFocusPolicy(Qt::StrongFocus);

    // ---- Fondo ----
    QPixmap bgOrig(":/resources/Torre_De_La_Marca.png");
    if (bgOrig.isNull()) qWarning() << "Error al cargar fondo";
    QPixmap bg = bgOrig.scaled(bgOrig.size() * 0.9,
                               Qt::KeepAspectRatioByExpanding,
                               Qt::SmoothTransformation);
    m_bgWidth  = bg.width();
    m_bgHeight = bg.height();
    m_scene->setSceneRect(0, 0, m_bgWidth * 2, m_bgHeight);

    for (int i = 0; i < m_repeatCount; ++i) {
        auto* itemBG = m_scene->addPixmap(bg);
        itemBG->setZValue(0);
        itemBG->setPos(i * m_bgWidth, 0);
    }
    // -- Segunda imagen, oculta al principio --
    QPixmap bg2Orig(":/resources/Torre_De_La_Marca2.png");
    if (bg2Orig.isNull()) {
        qWarning() << "Error al cargar raices_olvidadas2";
    } else {
        // escalado idéntico al primero
        QPixmap bg2 = bg2Orig.scaled(
            QSize(m_bgWidth, m_bgHeight),
            Qt::KeepAspectRatioByExpanding,
            Qt::SmoothTransformation
            );
        m_bg2Item = m_scene->addPixmap(bg2);
        m_bg2Item->setZValue(0);
        m_bg2Item->setPos(m_bgWidth, 0);
        m_bg2Item->setVisible(false);
    }




    // ---- View ----
    m_view = new QGraphicsView(m_scene, this);
    m_view->setFixedSize(int(WINDOW_W), int(WINDOW_H));
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setAttribute(Qt::WA_TransparentForMouseEvents);

    // ---- Plataformas ----
    float platX = 300.0f - (PLAT_WIDTH + 120.0f)/2.0f;
    float platY = (m_bgHeight - 40.0f) - 160.0f;
    m_colManager->addRect(
        QRectF(platX, platY,
               PLAT_WIDTH + 120.0f,
               PLAT_HEIGHT),
        QColor(80,80,80), false);
    m_colManager->addRect(
        QRectF(0.0f,
               m_bgHeight - 40.0f,
               float(m_bgWidth * 2),
               40.0f),
        Qt::NoBrush, true);


    // ---- Jugador ----
    if (m_player) {
        float footX =35;
        float footY = 0;
        m_player->transform().setPosition(footX, footY);
        m_player->setOnGround(true);
        m_spawnPos = QPointF(footX, footY);
        // Creamos el QGraphicsPixmapItem (vacío aún)
        m_playerItem = new QGraphicsPixmapItem;
        m_playerItem->setZValue(3);
        m_scene->addItem(m_playerItem);
        m_playerItem->setPos(footX, footY);
    }

    // ---- Mapa ----
    m_mapaRegiones = new MapaWidget("Raices Olvidadas", this);
    m_mapaRegiones->setWindowModality(Qt::NonModal);
    m_mapaRegiones->setFocusPolicy(Qt::NoFocus);
    m_mapaRegiones->setAttribute(Qt::WA_ShowWithoutActivating);

    activateWindow();
    setFocus(Qt::OtherFocusReason);

    connect(m_mapaRegiones, &MapaWidget::mapaCerrado, this, [this]() {
        activateWindow();
        setFocus(Qt::OtherFocusReason);
    });

    m_hudBorder = new QGraphicsRectItem(0, 0, HUD_W, HUD_H);
    m_hudBorder->setPen(QPen(Qt::black));
    m_hudBorder->setBrush(Qt::NoBrush);
    m_hudBorder->setZValue(100);  // siempre delante
    m_hudBorder->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_scene->addItem(m_hudBorder);

    // 2) Barra interior (verde)
    m_hudBar = new QGraphicsRectItem(1, 1, HUD_W-2, HUD_H-2);
    m_hudBar->setPen(Qt::NoPen);
    m_hudBar->setBrush(QColor(50,205,50));
    m_hudBar->setZValue(101);
    m_hudBar->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_scene->addItem(m_hudBar);

    // 3) Texto porcentaje
    m_hudText = new QGraphicsTextItem("100%");
    m_hudText->setDefaultTextColor(Qt::white);
    QFont fnt;
    fnt.setPointSize(14);
    m_hudText->setFont(fnt);
    m_hudText->setZValue(102);
    m_hudText->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_scene->addItem(m_hudText);

    connect(m_timer, &QTimer::timeout,
            this, &niveltorredelamarca::onFrame);
    m_timer->start(int(m_dt * 1000));
}

void niveltorredelamarca::keyPressEvent(QKeyEvent* event)
{
    if (m_deathScheduled) return;
    switch (event->key()) {
    case Qt::Key_A:     m_moveLeft      = true;  break;
    case Qt::Key_D:     m_moveRight     = true;  break;
    case Qt::Key_Shift: m_run           = true;  break;
    case Qt::Key_Space: m_jumpRequested = true;  break;
    case Qt::Key_C:
        if (m_player && m_player->isOnGround()) {
            float vx = m_player->fisica().velocity().x();
            auto  state = vx > 0.0f
                             ? SpriteState::Slidding
                             : SpriteState::SliddingLeft;
            m_player->reproducirAnimacionTemporal(state, 0.5f);
        }
        break;
    case Qt::Key_M:
        if (m_mapaRegiones) {
            if (!m_mapaRegiones->isVisible())
                m_mapaRegiones->show();
            else
                m_mapaRegiones->close();
        }
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void niveltorredelamarca::keyReleaseEvent(QKeyEvent* event)
{
    if (m_deathScheduled) return;
    switch (event->key()) {
    case Qt::Key_A:     m_moveLeft  = false; break;
    case Qt::Key_D:     m_moveRight = false; break;
    case Qt::Key_Shift: m_run        = false; break;
    default:
        QWidget::keyReleaseEvent(event);
    }
}

void niveltorredelamarca::mousePressEvent(QMouseEvent* event)
{

    if (!m_player || !m_player->isOnGround()) return;
    auto dir   = m_player->getLastDirection();
    auto state = (dir == SpriteState::WalkingLeft ||
                  dir == SpriteState::RunningLeft)
                     ? SpriteState::SlashingLeft
                     : SpriteState::Slashing;
    m_player->reproducirAnimacionTemporal(state, 0.6f);
}

void niveltorredelamarca::onFrame()
{
    if (!m_player) return;

    // --- PREPARAR SPRITE PARA DIBUJAR ---
    QSize sz = m_player->sprite().getSize();
    QPixmap frame = m_player->sprite()
                        .currentFrame()
                        .scaled(sz,
                                Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
    QPixmap spritePix = trimBottom(frame);

    // --- POSICIÓN DE LOS PIES ACTUAL ---
    QPointF footPos = m_player->transform().getPosition();

    // --- ZONA LETAL: SOLO SI NO HEMOS PROGRAMADO LA MUERTE ---
    if (!m_deathScheduled) {
        if (auto* jug = dynamic_cast<Jugador*>(m_player)) {
            float x = footPos.x();
            float y = footPos.y();
            constexpr float epsY = 1.0f;
            bool atY = (y >= 651.0f - epsY && y <= 651.0f + epsY);
            bool inFirst  = (x >=  304.33f && x <=  30967.33f);
            bool inSecond = (x >= 3371.0f  && x <=  3605.67f);

            if (atY && (inFirst || inSecond) && jug->currentHP() > 0) {
                // 1) Anulamos velocidad y aseguramos onGround
                m_player->fisica().setVelocity(0, 0);
                jug->setOnGround(true);

                // 2) Matamos al jugador: vida a 0 + animación
                jug->aplicarDano(jug->currentHP());
                jug->reproducirAnimacionTemporal(SpriteState::dead, 1.5f);
                m_deathScheduled = true;

                // 3) Tras 1.5s, ocultamos el sprite
                QTimer::singleShot(1000, this, [this]() {
                    m_playerItem->setVisible(false);
                });

                // 4) Tras 2s, respawneamos y limpiamos flags de movimiento
                QTimer::singleShot(2000, this, [this, jug]() {
                    // Reposicionamos al spawn original
                    m_player->transform().setPosition(35,0);
                    // Frenamos cualquier velocidad
                    m_player->fisica().setVelocity(0, 0);
                    // Marcamos en suelo
                    jug->setOnGround(true);
                    // Restablecemos animación y vida
                    jug->sprite().setState(SpriteState::Idle);
                    jug->setHP(jug->maxHP());
                    // Limpiamos los flags de input para evitar impulso
                    m_moveLeft  = false;
                    m_moveRight = false;
                    m_run       = false;
                    m_jumpRequested = false;
                    // Volvemos a mostrar el sprite
                    m_playerItem->setVisible(true);
                    // Permitimos muertes futuras
                    m_deathScheduled = false;
                });
            }
        }
    }

    // --- ENTRADA: solo si NO estamos en muerte programada ---
    float vx = 0;
    if (!m_deathScheduled) {
        // salto
        if (m_jumpRequested && m_player->isOnGround()) {
            constexpr float JUMP_SPEED = 500.0f;
            float vx0 = m_player->fisica().velocity().x();
            m_player->fisica().setVelocity(vx0, -JUMP_SPEED);
            m_player->setOnGround(false);
        }
        // movimiento horizontal
        if (m_moveLeft)  vx = -160.0f;
        if (m_moveRight) vx =  160.0f;
        if (m_run && vx != 0) vx *= 2;
    }
    // aplicamos siempre la componente horizontal
    m_player->fisica().setVelocity(
        vx,
        m_player->fisica().velocity().y()
        );
    m_jumpRequested = false;

    // --- FÍSICA y COLISIONES: SIEMPRE ---
    m_player->actualizar(m_dt);
    m_colManager->resolveCollisions(
        m_player,
        spritePix.size(),
        m_dt
        );

    // --- DIBUJO del sprite ---
    m_playerItem->setPixmap(spritePix);
    m_playerItem->setOffset(
        -spritePix.width() * 0.5f,
        -spritePix.height()
        );
    m_playerItem->setPos(footPos);

    // --- CÁMARA ---
    m_view->centerOn(footPos);

    // --- SEGUNDO FONDO si corresponde ---
    if (!m_secondBgShown &&
        footPos.x() >= m_bgWidth - WINDOW_W / 2.0f)
    {
        m_bg2Item->setVisible(true);
        m_secondBgShown = true;
    }

    // --- ACTUALIZAR HUD ---
    QPointF topLeft = m_view->mapToScene(0, 0);
    float fracVida = float(m_player->currentHP()) /
                     float(m_player->maxHP());

    m_hudBorder->setPos(
        topLeft.x() + HUD_MARGIN,
        topLeft.y() + HUD_MARGIN
        );
    m_hudBar->setRect(
        topLeft.x() + HUD_MARGIN + 1,
        topLeft.y() + HUD_MARGIN + 1,
        (HUD_W - 2) * fracVida,
        HUD_H - 2
        );
    int pct = qRound(fracVida * 100.0f);
    m_hudText->setPlainText(QString::number(pct) + "%");
    QRectF txtRect = m_hudText->boundingRect();
    m_hudText->setPos(
        topLeft.x() + HUD_MARGIN + (HUD_W - txtRect.width()) / 2.0f,
        topLeft.y() + HUD_MARGIN + (HUD_H - txtRect.height()) / 2.0f
        );
}


