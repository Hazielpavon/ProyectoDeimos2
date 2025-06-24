
#include "ciudadinversa.h"
#include "mapawidget.h"
#include "ObjetosYColisiones.h"
#include "jugador.h"
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
#include "mainwindow.h"
#include "MutantWorm.h"
#include "MonsterFly.h"
#include "Minotaur.h"
#include "drop.h"
#include <QGraphicsTextItem>
#include <QFont>
#include <QBrush>


static constexpr float WINDOW_W   = 950.0f;
static constexpr float WINDOW_H   = 650.0f;
static constexpr float FPS        = 60.0f;

static constexpr float HUD_W = 350.0f;
static constexpr float HUD_H = 35.0f;
static constexpr float HUD_MARGIN = 10.0f;

static QPixmap trimBottom(const QPixmap& pix)
{
    QImage img = pix.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
    int maxY = -1;
    for (int y=0; y<img.height(); ++y)
        for (int x=0; x<img.width(); ++x)
            if (qAlpha(img.pixel(x,y))>0) maxY = std::max(maxY,y);

    return (maxY>=0 && maxY<img.height()-1)
               ? pix.copy(0,0,pix.width(),maxY+1)
               : pix;
}

static inline void placeSprite(QGraphicsPixmapItem* it,
                               qreal left, qreal top,
                               qreal bgHeight)
{
    it->setPos(left, bgHeight - top - it->pixmap().height());
}

ciudadinversa::ciudadinversa(entidad*   jugador,
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
    setFocus();

    QPixmap bgOrig(":/resources/fondociudadinversa1.jpg");
    QPixmap bg = bgOrig.scaled(bgOrig.size()*2,
                               Qt::KeepAspectRatioByExpanding,
                               Qt::SmoothTransformation);

    m_bgWidth  = bg.width();
    m_bgHeight = bg.height();
    m_scene->setSceneRect(0, 0, m_bgWidth*2, m_bgHeight);

    for (int i = 0; i < 2; ++i) {
        auto* pm = m_scene->addPixmap(bg);
        pm->setZValue(0);
        pm->setPos(i * m_bgWidth, 0);
    }

    if (QPixmap bg2(":/resources/fondociudadinversa2.jpg"); !bg2.isNull()) {
        m_bg2Item = m_scene->addPixmap(
            bg2.scaled(m_bgWidth, m_bgHeight,
                       Qt::KeepAspectRatioByExpanding,
                       Qt::SmoothTransformation));
        m_bg2Item->setPos(m_bgWidth, 0);
        m_bg2Item->setZValue(0);
        m_bg2Item->setVisible(false);
    }



    m_view = new QGraphicsView(m_scene,this);
    m_view->setFixedSize(int(WINDOW_W),int(WINDOW_H));
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_view->scale(1.0,-1.0);
    m_view->translate(0,-m_bgHeight);

    QGraphicsTextItem* textoMagico = new QGraphicsTextItem();

    textoMagico->setPlainText("aqui todo va hacia arriba, ten cuidado...\nla magia oscura bloquea tu propia magia");

    QFont fuenteFantasy("Papyrus", 14);
    fuenteFantasy.setBold(true);
    textoMagico->setFont(fuenteFantasy);

    textoMagico->setDefaultTextColor(Qt::white);

    textoMagico->setPos(196, 320);

    textoMagico->setZValue(5);

    QTransform t;
    t.scale(1, -1);
    textoMagico->setTransform(t);

    m_scene->addItem(textoMagico);

    constexpr float WALL = 40.f;
    m_colManager->addRect({0, 0,               qreal(m_bgWidth*2),WALL}, Qt::NoBrush,true);
    m_colManager->addRect({0, m_bgHeight-WALL, qreal(m_bgWidth*2),WALL}, Qt::NoBrush,true);
    m_colManager->addRect({0, 0,               WALL, qreal(m_bgHeight)}, Qt::NoBrush,true);
    m_colManager->addRect({m_bgWidth*2-WALL,0, WALL, qreal(m_bgHeight)}, Qt::NoBrush,true);


    static constexpr float PLAT_W = 200.f;
    static constexpr float PLAT_H =  20.f;

    const QVector<QRectF> plataformas = {
        { 100.f, 20.f, PLAT_W, PLAT_H },
        { 500.f, 150.f, PLAT_W, PLAT_H },
        { 900.f, 280.f, PLAT_W, PLAT_H },
        { 1200.f, 160.f, PLAT_W, PLAT_H },
        { 1600.f, 80.f, PLAT_W, PLAT_H },
        { 1900.f, 130.f, PLAT_W, PLAT_H },
        { 2200.f, 240.f, PLAT_W, PLAT_H },
        { 2500.f, 320.f, PLAT_W, PLAT_H },
        { 2900.f, 70.f, PLAT_W, PLAT_H },
        { 3200.f, 120.f, PLAT_W, PLAT_H },
        { 3500.f, 220.f, PLAT_W, PLAT_H },
        { 3900.f, 120.f, PLAT_W, PLAT_H },
        { 4300.f, 200.f, PLAT_W, PLAT_H },
        { 4700.f, 300.f, PLAT_W, PLAT_H },
        { 5200.f, 30.f, 2000.f, PLAT_H }
    };

    QPixmap texPlat(":/resources/plataforma_normal.png");

    for (const QRectF& r : plataformas)
    {
        QPixmap px = texPlat.scaled(int(r.width()), int(r.height()),
                                    Qt::IgnoreAspectRatio,
                                    Qt::SmoothTransformation);
        auto* spr = m_scene->addPixmap(px);
        spr->setZValue(1);
        placeSprite(spr, r.x(), r.y(), m_bgHeight);

        QRectF hitRect(
            r.x(),
            m_bgHeight - r.y() - r.height(),
            r.width(),
            r.height()
            );
        m_colManager->addRect(hitRect, Qt::NoBrush,true);
    }

    {
        constexpr qreal KILL_X0 = 382.0;
        constexpr qreal KILL_X1 = 5142.0;
        constexpr qreal KILL_Y  = 60.0;
        constexpr qreal KILL_H  = 10.0;

        QRectF killRect(KILL_X0,
                        m_bgHeight - KILL_Y,
                        KILL_X1 - KILL_X0,
                        KILL_H);

        m_killZone = m_scene->addRect(killRect, Qt::NoPen, Qt::NoBrush);
        m_killZone->setZValue(0);
    }



    if(m_player){
        m_spawnPos = {100.f, 400.f};
        m_player->transform().setPosition(m_spawnPos.x(),m_spawnPos.y());
        m_player->setOnGround(true);

        m_playerItem = new QGraphicsPixmapItem;
        m_playerItem->setZValue(3);
        m_scene->addItem(m_playerItem);
        m_playerItem->setPos(m_spawnPos);

        if(auto* jug = dynamic_cast<Jugador*>(m_player))
            jug->setGraphicsItem(m_playerItem);
    }

    if(auto* jug = dynamic_cast<Jugador*>(m_player))
        m_combate = new CombateManager(jug, m_enemigos, this);

    {
        const QVector<QPointF> spawnPts = {
            {1018, 300},   // 415 - 5
            {1312, 400},   // 535 - 5
            {1712, 500},   // 615 - 5
            {2010, 440},   // 564 - 5
            {2315, 330},   // 454 - 5
            {2620, 220},   // 375 - 5
            {3005, 500},   // 625 - 5
            {3328, 450},   // 575 - 5
            {3630, 320},   // 475 - 5
            {4016, 400},   // 575 - 5
            {4426, 300},   // 495 - 5
            {4822, 200}    // 395 - 5
        };

        for (const QPointF& p : spawnPts)
        {
            auto* w = new MutantWorm(this);
            w->setStationary(true);
            const qreal h = w->boundingRect().height();
            w->setPos(p.x(), p.y() + h * 0.5);
            m_scene->addItem(w);

            w->setTarget(m_player);
            m_enemigos.append(w);
        }
    }

    {
        auto* mino = new Minotaur(this);
        mino->setPos(5870.f, 520.f);
        mino->setTarget(m_player);
        m_scene->addItem(mino);
        m_enemigos.append(mino);
    }


    if (auto* jug = dynamic_cast<Jugador*>(m_player))
        m_combate = new CombateManager(jug, m_enemigos, this);


    m_hudBorder = new QGraphicsRectItem(0,0,HUD_W,HUD_H);
    m_hudBorder->setPen(QPen(Qt::black));
    m_hudBorder->setBrush(Qt::NoBrush);
    m_hudBorder->setZValue(100);
    m_scene->addItem(m_hudBorder);

    m_hudBar = new QGraphicsRectItem(1,1,HUD_W-2,HUD_H-2);
    m_hudBar->setPen(Qt::NoPen);
    m_hudBar->setBrush(QColor(50,205,50));
    m_hudBar->setZValue(101);
    m_scene->addItem(m_hudBar);

    m_hudText = new QGraphicsTextItem("100%");
    QFont f; f.setPointSize(14);
    m_hudText->setFont(f);
    m_hudText->setDefaultTextColor(Qt::white);
    m_hudText->setZValue(102);
    m_hudText->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_scene->addItem(m_hudText);

    connect(m_timer,&QTimer::timeout,this,&ciudadinversa::onFrame);
    m_timer->start(int(m_dt*1000));




}


void ciudadinversa::keyPressEvent(QKeyEvent* e)
{
    if (m_deathScheduled) return;

    switch (e->key()) {
    case Qt::Key_A:     m_moveLeft  = true;  break;
    case Qt::Key_D:     m_moveRight = true;  break;
    case Qt::Key_Shift: m_run       = true;  break;

    case Qt::Key_Space:
        if (!e->isAutoRepeat()) {
            m_jumpRequested = true;
        }
        break;

    case Qt::Key_M:
        m_mapaRegiones->setVisible(!m_mapaRegiones->isVisible());
        break;

    case Qt::Key_C:
        if (m_player && m_player->isOnGround()) {
            float vx = m_player->fisica().velocity().x();
            SpriteState st = vx>0.0f ? SpriteState::Slidding
                                       : SpriteState::SliddingLeft;
            m_player->reproducirAnimacionTemporal(st,0.5f);
        }
        break;

    default:
        QWidget::keyPressEvent(e);
    }
}

void ciudadinversa::keyReleaseEvent(QKeyEvent* e)
{
    if(m_deathScheduled) return;
    switch(e->key()){
    case Qt::Key_A:     m_moveLeft  = false; break;
    case Qt::Key_D:     m_moveRight = false; break;
    case Qt::Key_Shift: m_run       = false; break;
    default: QWidget::keyReleaseEvent(e);
    }
}
void ciudadinversa::mousePressEvent(QMouseEvent*)
{
    if(!m_player || !m_player->isOnGround()) return;
    SpriteState st = (m_player->getLastDirection()==SpriteState::WalkingLeft ||
                      m_player->getLastDirection()==SpriteState::RunningLeft)
                         ? SpriteState::SlashingLeft
                         : SpriteState::Slashing;
    m_player->reproducirAnimacionTemporal(st,0.6f);
}

void ciudadinversa::onFrame()
{
    if (!m_player) return;

    {
        const QPointF pos = m_player->transform().getPosition();
        qDebug() << "[Player] x =" << pos.x() << "y =" << pos.y();
    }


    QPointF footPos1 = m_player->transform().getPosition();
    float x = footPos1.x();
    float y = footPos1.y();

    if (x < 0.0f) {
        m_timer->stop();
        m_mainWindow->cargarNivel("RaicesOlvidadas");
        return;
    }


    if (x >= 6245.67f) {
        m_timer->stop();
        m_mainWindow->cargarNivel("MenteVacia");
        return;
    }

    if (!m_deathScheduled && m_player->currentHP() <= 0) {
        auto* jug = dynamic_cast<Jugador*>(m_player);
        m_player->fisica().setVelocity(0, 0);
        jug->setOnGround(true);
        jug->reproducirAnimacionTemporal(SpriteState::dead, 1.5f);
        m_deathScheduled = true;
        QTimer::singleShot(1000, this, [this]() { m_playerItem->setVisible(false); });
        QTimer::singleShot(2000, this, [this, jug]() {
            m_player->transform().setPosition(m_spawnPos.x(), m_spawnPos.y());
            m_player->fisica().setVelocity(0, 0);
            jug->setOnGround(true);
            jug->sprite().setState(SpriteState::Idle);
            jug->setHP(jug->maxHP());

            m_moveLeft = m_moveRight = m_run = m_jumpRequested = false;
            m_playerItem->setVisible(true);
            m_deathScheduled = false;
        });
        return;
    }

    if (m_jumpRequested && m_player->isOnGround()) {
        constexpr float JUMP_VY = -500.0f;
        auto v = m_player->fisica().velocity();
        m_player->fisica().setVelocity(v.x(), JUMP_VY);
        m_player->setOnGround(false);
        m_jumpRequested = false;
    }
    float vx = 0.0f;
    if (!m_deathScheduled) {
        if (m_moveLeft)  vx = -160.0f;
        if (m_moveRight) vx =  160.0f;
        if (m_run && vx != 0.0f) vx *= 2.0f;
    }
    m_player->fisica().setVelocity(
        vx,
        m_player->fisica().velocity().y()
        );

    m_player->actualizar(m_dt);
    QSize sprSz = m_player->sprite().getSize();
    m_colManager->resolveCollisions(m_player, sprSz, m_dt);

    if (!m_secondBgShown &&
        m_player->transform().getPosition().x() >= (m_bgWidth - WINDOW_W/2.0f))
    {
        m_bg2Item->setVisible(true);
        m_secondBgShown = true;
    }
    for (Enemigo* e : std::as_const(m_enemigos)) {
        e->update(m_dt);
        if (!m_deadDrops.contains(e) && e->isDead())
        {
            if (dynamic_cast<MutantWorm*>(e))
            {
                QPointF dropPos = e->pos();

                Drop* d = new Drop(Drop::Tipo::Vida,
                                   dropPos,
                                   m_scene,QString(),
                                   this);

                m_drops.append(d);
            }
            m_deadDrops.insert(e);
        }



        if ( e->isDead() &&
            !e->property("dropDone").toBool() &&
            dynamic_cast<Minotaur*>(e) )
        {
            QPointF pos = e->pos();

            auto* dVida = new Drop(Drop::Tipo::Vida,
                                   pos,
                                   m_scene,
                                   QString(),
                                   this);
            m_drops.append(dVida);

            auto* dKey  = new Drop(Drop::Tipo::Llave,
                                  pos + QPointF(25,0),
                                  m_scene,
                                  "LlaveMinotauro",
                                  this);
            m_drops.append(dKey);

            e->setProperty("dropDone", true);
        }

        QSize eSz = e->pixmap().size();
        m_colManager->resolveCollisions(e, eSz, m_dt);
    }

    if (m_bossHpBorder && m_bossHpBar && m_debugBossHitbox &&
        !m_enemigos.isEmpty()) {
        Enemigo* boss = m_enemigos.first();
        QRectF sb = boss->sceneBoundingRect();

        m_debugBossHitbox->setRect(0, 0, sb.width(), sb.height());
        m_debugBossHitbox->setPos(sb.topLeft());

        float frac = float(boss->currentHP()) / boss->maxHP();
        float bw = m_bossHpBorder->rect().width();
        float bh = m_bossHpBorder->rect().height();
        float x0 = sb.left() + (sb.width() - bw)/2.0f;
        float y0 = sb.top()  - bh - 4.0f;
        m_bossHpBorder->setRect(0,0,bw,bh);
        m_bossHpBorder->setPos(x0,y0);
        m_bossHpBar->setRect(1,1,(bw-2)*frac, bh-2);
        m_bossHpBar->setPos(x0,y0);
    }

    if (m_combate) m_combate->update(m_dt);

    for (int i = m_drops.size() - 1; i >= 0; --i)
    {
        Drop* d = m_drops[i];
        if (!d) { m_drops.remove(i); continue; }


        if (d->isCollected()) {
            delete d;
            m_drops.remove(i);
            continue;
        }

        if (d->checkCollision(m_player)) {
            d->aplicarEfecto(m_player);
            delete d;
            m_drops.remove(i);
        }
    }


    QPixmap pix = trimBottom(
        m_player->sprite().currentFrame()
            .scaled(sprSz, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    QPointF footPos = m_player->transform().getPosition();
    m_playerItem->setPixmap(pix);
    m_playerItem->setOffset(-pix.width()/2.0, -pix.height());
    m_playerItem->setPos(footPos);
    m_view->centerOn(footPos);


    const QPointF anchor = m_view->mapToScene(
        0, WINDOW_H - HUD_H - HUD_MARGIN);

    const qreal left = anchor.x() + HUD_MARGIN;
    const qreal top  = anchor.y();

    float hpFrac = float(m_player->currentHP()) / m_player->maxHP();

    m_hudBorder->setRect(0, 0, HUD_W, HUD_H);
    m_hudBorder->setPos(left, top);

    m_hudBar->setPos(left, top);
    m_hudBar->setRect(1, 2,
                      (HUD_W - 2) * hpFrac,
                      HUD_H - 3);
    m_hudBar->setBrush(QColor(50, 205, 50));


    if (m_killZone && m_playerItem->collidesWithItem(m_killZone))
    {
        m_player->setHP(0);
    }

    int hpPct = int(hpFrac * 100.f + 0.5f);

}
