#include "nivelraicesolvidadas.h"
#include "mapawidget.h"
#include "ObjetosYColisiones.h"
#include "jugador.h"
#include "Enemigo.h"
#include "BringerOfDeath.h"
#include "CombateManager.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QFrame>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QImage>
#include <algorithm>
#include <QDebug>

// ---- Constantes generales --------------------------------
static constexpr float WINDOW_W    = 950.0f;
static constexpr float WINDOW_H    = 650.0f;
static constexpr float FPS         = 60.0f;
static constexpr float PLAT_WIDTH  = 200.0f;
static constexpr float PLAT_HEIGHT = 20.0f;

// HUD
static constexpr float HUD_W = 350.0f;
static constexpr float HUD_H = 35.0f;
static constexpr float HUD_MARGIN = 10.0f;

/* Auxiliar: recorta líneas transparentes inferiores */
static QPixmap trimBottom(const QPixmap& pix)
{
    QImage img = pix.toImage()
    .convertToFormat(QImage::Format_ARGB32_Premultiplied);
    int maxY = -1;
    for (int y=0; y<img.height(); ++y)
        for (int x=0; x<img.width(); ++x)
            if (qAlpha(img.pixel(x,y))>0) maxY = std::max(maxY,y);
    return (maxY>=0 && maxY<img.height()-1)
               ? pix.copy(0,0,pix.width(),maxY+1)
               : pix;
}

// =========================================================
NivelRaicesOlvidadas::NivelRaicesOlvidadas(entidad*   jugador,
                                           MainWindow* mainWindow,
                                           QWidget*   parent)
    : QWidget(parent)
    , m_player(jugador)
    , m_mainWindow(mainWindow)
    , m_timer(new QTimer(this))
    , m_scene(new QGraphicsScene(this))
    , m_colManager(new ObjetosYColisiones(m_scene, this))
    , m_dt(1.0f/FPS)
{
    setFixedSize(int(WINDOW_W), int(WINDOW_H));
    setFocusPolicy(Qt::StrongFocus);

    // ---- Fondo ----
    QPixmap bgOrig(":/resources/raices_olvidadas.png");
    QPixmap bg = bgOrig.scaled(bgOrig.size()*0.9,
                               Qt::KeepAspectRatioByExpanding,
                               Qt::SmoothTransformation);
    m_bgWidth  = bg.width();
    m_bgHeight = bg.height();
    m_scene->setSceneRect(0,0,m_bgWidth*2, m_bgHeight);
    for (int i=0;i<2;++i){
        auto* item = m_scene->addPixmap(bg);
        item->setZValue(0);  item->setPos(i*m_bgWidth,0);
    }
    QPixmap bg2Orig(":/resources/raices_olvidadas2.png");
    if(!bg2Orig.isNull()){
        m_bg2Item = m_scene->addPixmap(
            bg2Orig.scaled(m_bgWidth, m_bgHeight,
                           Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation));
        m_bg2Item->setPos(m_bgWidth,0);
        m_bg2Item->setZValue(0);
        m_bg2Item->setVisible(false);
    }

    // ---- Vista ----
    m_view = new QGraphicsView(m_scene, this);
    m_view->setFixedSize(int(WINDOW_W), int(WINDOW_H));
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setAttribute(Qt::WA_TransparentForMouseEvents);

    // ---- Plataformas / suelo ----
    float platX = 300.0f - (PLAT_WIDTH+120.0f)/2.0f;
    float platY = (m_bgHeight-40.0f) - 160.0f;
    m_colManager->addRect({platX, platY,
                           PLAT_WIDTH+120.0f, PLAT_HEIGHT},
                          QColor(80,80,80), false);
    m_colManager->addRect({0.0f, m_bgHeight-40.0f,
                           float(m_bgWidth*2),40.0f},
                          Qt::NoBrush, true);

    // ---- Jugador ----
    if(m_player){
        m_spawnPos = QPointF(35,0);
        m_player->transform().setPosition(
            m_spawnPos.x(), m_spawnPos.y());
        m_player->setOnGround(true);

        m_playerItem = new QGraphicsPixmapItem;
        m_playerItem->setZValue(3);
        m_scene->addItem(m_playerItem);
        m_playerItem->setPos(m_spawnPos);
        auto jug = dynamic_cast<Jugador*>(m_player);
        if (jug) jug->setGraphicsItem(m_playerItem);
    }

    // ---- Enemigo (Bringer-of-Death) ----
    auto* boss = new BringerOfDeath(this);
    QSize bSz  = boss->pixmap().size();
    boss->setPos(platX+(PLAT_WIDTH+120.0f)/2.0f,
                 platY - bSz.height()/2.0f);
    boss->setTarget(m_player);
    m_scene->addItem(boss);
    m_enemigos.append(boss);
    m_debugBossHitbox = new QGraphicsRectItem;
    m_debugBossHitbox->setPen(QPen(Qt::red, 2, Qt::DashLine));
    m_debugBossHitbox->setBrush(Qt::NoBrush);
    m_debugBossHitbox->setZValue(10);  // por encima para que siempre se vea
    m_scene->addItem(m_debugBossHitbox);

    // ---- Gestor de Combate ----
    Jugador* jugadorPtr = dynamic_cast<Jugador*>(m_player);
    if (!jugadorPtr) {
        qCritical() << "[NivelRaicesOlvidadas] m_player no es Jugador!";
    } else {
        m_combate = new CombateManager(jugadorPtr, m_enemigos, this);
    }

    // ---- Mapa + HUD (igual que antes) ----
    m_mapaRegiones = new MapaWidget("Raices Olvidadas", this);
    connect(m_mapaRegiones,&MapaWidget::mapaCerrado,
            this,[this](){ activateWindow(); setFocus(); });

    m_hudBorder = new QGraphicsRectItem(0,0,HUD_W,HUD_H);
    m_hudBorder->setPen(QPen(Qt::black));
    m_hudBorder->setBrush(Qt::NoBrush);
    m_hudBorder->setZValue(100);
    m_hudBorder->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_scene->addItem(m_hudBorder);

    m_hudBar = new QGraphicsRectItem(1,1,HUD_W-2,HUD_H-2);
    m_hudBar->setPen(Qt::NoPen);
    m_hudBar->setBrush(QColor(50,205,50));
    m_hudBar->setZValue(101);
    m_hudBar->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_scene->addItem(m_hudBar);

    m_hudText = new QGraphicsTextItem("100%");
    QFont f; f.setPointSize(14);
    m_hudText->setFont(f);
    m_hudText->setDefaultTextColor(Qt::white);
    m_hudText->setZValue(102);
    m_hudText->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_scene->addItem(m_hudText);

    connect(m_timer,&QTimer::timeout,this,&NivelRaicesOlvidadas::onFrame);
    m_timer->start(int(m_dt*1000));
}

/* =========================================================//---------------------------------------------------------------------------------------
 *  Entrada (no cambia)
 * ========================================================= */
void NivelRaicesOlvidadas::keyPressEvent(QKeyEvent* e)
{
    if(m_deathScheduled) return;
    switch(e->key()){
    case Qt::Key_A:     m_moveLeft  = true;  break;
    case Qt::Key_D:     m_moveRight = true;  break;
    case Qt::Key_Shift: m_run       = true;  break;
    case Qt::Key_Space: m_jumpRequested = true; break;
    case Qt::Key_M:
        m_mapaRegiones->setVisible(!m_mapaRegiones->isVisible());
        break;
    case Qt::Key_C:
        if(m_player && m_player->isOnGround()){
            float vx = m_player->fisica().velocity().x();
            SpriteState st = vx>0.0f ? SpriteState::Slidding
                                       : SpriteState::SliddingLeft;
            m_player->reproducirAnimacionTemporal(st,0.5f);
        }
        break;
    default: QWidget::keyPressEvent(e);
    }
}
void NivelRaicesOlvidadas::keyReleaseEvent(QKeyEvent* e)
{
    if(m_deathScheduled) return;
    switch(e->key()){
    case Qt::Key_A:     m_moveLeft  = false; break;
    case Qt::Key_D:     m_moveRight = false; break;
    case Qt::Key_Shift: m_run       = false; break;
    default: QWidget::keyReleaseEvent(e);
    }
}
void NivelRaicesOlvidadas::mousePressEvent(QMouseEvent*)
{
    if(!m_player || !m_player->isOnGround()) return;
    SpriteState st = (m_player->getLastDirection()==SpriteState::WalkingLeft ||
                      m_player->getLastDirection()==SpriteState::RunningLeft)
                         ? SpriteState::SlashingLeft
                         : SpriteState::Slashing;
    m_player->reproducirAnimacionTemporal(st,0.6f);
}

/* =========================================================
 *  Loop principal
 * ========================================================= */
void NivelRaicesOlvidadas::onFrame()
{
    if(!m_player) return;

    /* ——— Entrada salto + movimiento horiz ——— */
    if(!m_deathScheduled && m_jumpRequested && m_player->isOnGround()){
        m_player->fisica().setVelocity(
            m_player->fisica().velocity().x(), -500.0f);
        m_player->setOnGround(false);
    }
    m_jumpRequested = false;

    float vx = 0.0f;
    if(!m_deathScheduled){
        if(m_moveLeft)  vx = -160.0f;
        if(m_moveRight) vx =  160.0f;
        if(m_run && vx!=0) vx *= 2.0f;
    }
    m_player->fisica().setVelocity(vx,
                                   m_player->fisica().velocity().y());

    m_player->actualizar(m_dt);
    QSize sprSz = m_player->sprite().getSize();
    m_colManager->resolveCollisions(m_player, sprSz, m_dt);

    /* ——— Enemigos ——— */
    // ‣ Iteramos usando qAsConst para no invalidar el contenedor
    // ——— física del jugador ———a
    for (Enemigo* e : qAsConst(m_enemigos)) {
        e->update(m_dt);
        QSize  eSz = e->pixmap().size();
        QRectF rectE(e->pos().x() - eSz.width()/2.0,
                     e->pos().y() - eSz.height()/2.0,
                     eSz.width(), eSz.height());
        QRectF rectPlat(300.0-60.0f,
                        (m_bgHeight-40.0f)-160.0f,
                        PLAT_WIDTH+120.0f, PLAT_HEIGHT);
        float sueloY = m_bgHeight-40.0f;
        float pie    = e->pos().y() + eSz.height()/2.0f;
        if (!m_enemigos.isEmpty() && m_debugBossHitbox) {
            Enemigo* boss = m_enemigos.first();  // asumimos que el jefe es el primero
            QRectF bb = boss->sceneBoundingRect();
            m_debugBossHitbox->setRect(bb);
        }
        // Plataforma
        if (rectE.intersects(rectPlat) &&
            e->velY() >= 0.0f &&
            pie >= rectPlat.top())
        {
            e->setPos(e->pos().x(),
                      rectPlat.top() - eSz.height()/2.0f - 1);
            e->setVelY(0.0f);
        }
        // Suelo
        else if (pie >= sueloY) {
            e->setPos(e->pos().x(),
                      sueloY - eSz.height()/2.0f - 1);
            e->setVelY(0.0f);
        }
    }

    /* ——— Combate ——— */
    if (m_combate) m_combate->update(m_dt);

    /* ——— Render jugador + cámara ——— */
    QPixmap pix = trimBottom(
        m_player->sprite().currentFrame()
            .scaled(sprSz, Qt::KeepAspectRatio,
                    Qt::SmoothTransformation));
    QPointF footPos = m_player->transform().getPosition();
    m_playerItem->setPixmap(pix);
    m_playerItem->setOffset(-pix.width()/2.0, -pix.height());
    m_playerItem->setPos(footPos);
    m_view->centerOn(footPos);

    /* ——— Fondo secundario ——— */
    if(!m_secondBgShown &&
        footPos.x() >= m_bgWidth - WINDOW_W/2.0f){
        if(m_bg2Item) m_bg2Item->setVisible(true);
        m_secondBgShown = true;
    }

    /* ——— HUD ——— */
    QPointF tl = m_view->mapToScene(0,0);
    float frac = float(m_player->currentHP())/
                 float(m_player->maxHP());
    m_hudBorder->setPos(tl.x()+HUD_MARGIN, tl.y()+HUD_MARGIN);
    m_hudBar->setRect(tl.x()+HUD_MARGIN+1, tl.y()+HUD_MARGIN+1,
                      (HUD_W-2)*frac, HUD_H-2);
    int pct = int(frac*100.0f + 0.5f);
    m_hudText->setPlainText(QString::number(pct)+"%");
    QRectF rTxt = m_hudText->boundingRect();
    m_hudText->setPos(tl.x()+HUD_MARGIN+(HUD_W-rTxt.width())/2.0f,
                      tl.y()+HUD_MARGIN+(HUD_H-rTxt.height())/2.0f);
}
