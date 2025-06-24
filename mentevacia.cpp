
#include "mapawidget.h"
#include "ObjetosYColisiones.h"
#include "jugador.h"
#include <QRandomGenerator>
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
#include "Skeleton.h"
#include "monsterfly.h"
#include "MutantWorm.h"
#include "mentevacia.h"
#include "npc.h"
#include "mainwindow.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QRect>
#include <QPolygonF>

static constexpr float WINDOW_W    = 950.0f;
static constexpr float WINDOW_H    = 650.0f;
static constexpr float FPS         = 60.0f;
static constexpr float PLAT_WIDTH  = 200.0f;
static constexpr float PLAT_HEIGHT = 20.0f;

static constexpr float HUD_W = 350.0f;
static constexpr float HUD_H = 35.0f;
static constexpr float HUD_MARGIN = 10.0f;

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

void mentevacia::rewardPlayerExtraDamage() {
    auto* jug = dynamic_cast<Jugador*>(m_player);
    if (!jug) return;
    jug->setDamageMultiplier( jug->damageMultiplier() * 1.05f );
}
mentevacia::mentevacia(entidad*   jugador,
                                         MainWindow* mainWindow,
                                         QWidget*   parent)
    : QWidget(parent)
    , m_player(jugador)
    , m_mainWindow(mainWindow)
    , m_timer(new QTimer(this))
    , m_scene(new QGraphicsScene(this))
    , m_colManager(new ObjetosYColisiones(m_scene, this))
    , m_dt(1.0f/FPS)
    , m_currentRound(0)
    , m_maxRounds(10)

{
    setFixedSize(int(WINDOW_W), int(WINDOW_H));
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    QPixmap bgOrig(":/resources/MenteVacia.png");
    QPixmap bg = bgOrig.scaled(bgOrig.size()*0.9,
                               Qt::KeepAspectRatioByExpanding,
                               Qt::SmoothTransformation);
    m_bgWidth  = bg.width();
    m_bgHeight = bg.height();
    float groundY = m_bgHeight;
    float skyY    = 0;

    QBrush brush(bg);
    m_scene->setBackgroundBrush(brush);
    m_scene->setSceneRect(0, 0, bg.width(), bg.height());

    m_view = new QGraphicsView(m_scene, this);
    m_view->setFixedSize(int(WINDOW_W), int(WINDOW_H));
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setAttribute(Qt::WA_TransparentForMouseEvents);



    float startX = 299.0f;
    float endX   = 3592.33f;
    float minY   = 200.0f;
    float maxY   = m_bgHeight - 40.0f - 100.0f;
    int   count  = 15;
    float minGapX = 50.0f;
    float minGapY = 40.0f;


    m_colManager->addRect({0.0f, m_bgHeight-40.0f,float(m_bgWidth*2),40.0f}, Qt::NoBrush, true);

    m_leftWall = m_colManager->addRect(
        QRectF(0, 0, 1.0f, m_bgHeight),
        Qt::NoBrush, true
        );

    m_rightWall = m_colManager->addRect(
        QRectF(m_bgWidth-1.0f, 0, 1.0f, m_bgHeight),
        Qt::NoBrush, true
        );


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

    m_debugBossHitbox = new QGraphicsRectItem;
    m_debugBossHitbox->setPen(QPen(Qt::red,2,Qt::DashLine));
    m_debugBossHitbox->setBrush(Qt::NoBrush);
    m_debugBossHitbox->setZValue(10);


    float barW = 100, barH = 8;
    m_bossHpBorder = new QGraphicsRectItem(0,0, barW, barH);
    m_bossHpBorder->setPen(QPen(Qt::black));
    m_bossHpBorder->setBrush(Qt::NoBrush);
    m_bossHpBorder->setZValue(11);
    m_bossHpBar = new QGraphicsRectItem(1,1, barW-2, barH-2);
    m_bossHpBar->setPen(Qt::NoPen);
    m_bossHpBar->setBrush(QColor(200,0,0));
    m_bossHpBar->setZValue(12);
    m_scene->addItem(m_bossHpBorder);
    m_scene->addItem(m_bossHpBar);

    m_hudManaBorder = new QGraphicsRectItem(0, 0, HUD_W, HUD_H);
    m_hudManaBorder->setPen(QPen(Qt::black));
    m_hudManaBorder->setBrush(Qt::NoBrush);
    m_hudManaBorder->setZValue(100);
    m_hudManaBorder->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_scene->addItem(m_hudManaBorder);

    m_hudManaBar = new QGraphicsRectItem(1, 1, HUD_W - 2, HUD_H - 2);
    m_hudManaBar->setPen(Qt::NoPen);
    m_hudManaBar->setBrush(QColor(0, 0, 255));  // Azul
    m_hudManaBar->setZValue(101);
    m_hudManaBar->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_scene->addItem(m_hudManaBar);

    m_manaText = new QGraphicsTextItem("100%");
    QFont f2; f2.setPointSize(14);
    m_manaText->setFont(f2);
    m_manaText->setDefaultTextColor(Qt::white);
    m_manaText->setZValue(102);
    m_manaText->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_scene->addItem(m_manaText);

    Jugador* jugadorPtr = dynamic_cast<Jugador*>(m_player);
    if (!jugadorPtr) {
        qCritical() << "[NivelRaicesOlvidadas] m_player no es Jugador!";
    } else {
        m_combate = new CombateManager(jugadorPtr, m_enemigos, this);
    }

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



    m_roundLabel = new QGraphicsTextItem;
    QFont fo; fo.setPointSize(48); fo.setBold(true);
    m_roundLabel->setFont(fo);
    m_roundLabel->setDefaultTextColor(QColor(200,180,0,220));
    m_roundLabel->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_roundLabel->setZValue(200);
    m_roundLabel->setVisible(false);
    m_scene->addItem(m_roundLabel);

    m_roundLabelTimer = new QTimer(this);
    m_roundLabelTimer->setSingleShot(true);
    connect(m_roundLabelTimer, &QTimer::timeout, this, [this]() {

        m_roundLabel->setVisible(false);

        spawnScaledEnemies(m_currentRound);
        m_roundActive = true;
        updateInfoText();
    });


    QFont infoFont;
    m_infoText = new QGraphicsTextItem;
    infoFont.setPointSize(12);
    m_infoText->setFont(infoFont);
    m_infoText->setDefaultTextColor(Qt::white);
    m_infoText->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_scene->addItem(m_infoText);
    m_infoText->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_infoText->setPos(10, HUD_MARGIN + HUD_H*2 + 4);

    m_scene->addItem(m_infoText);





    connect(m_timer, &QTimer::timeout, this, &mentevacia::onFrame);
    startNextRound();
    m_timer->start(int(m_dt*1000));
}

void mentevacia::startNextRound()
{
    ++m_currentRound;
    if (m_currentRound > m_maxRounds) {
        m_timer->stop();
        return;
    }

    if (m_currentRound == 10) {
        m_roundLabel->setPlainText(QStringLiteral("¡RONDA 10! Ya puedes avanzar"));
        m_roundLabel->setVisible(true);
        m_roundLabelTimer->start(3000);

        if (m_leftWall) {
            m_colManager->removeRect(m_leftWall);
            m_leftWall = nullptr;
        }
        if (m_rightWall) {
            m_colManager->removeRect(m_rightWall);
            m_rightWall = nullptr;
        }


    }
    m_roundActive = false;
    showRoundLabel(m_currentRound);
}
void mentevacia::showRoundLabel(int round)
{
    m_roundLabel->setPlainText(QStringLiteral("RONDA %1").arg(round));
    m_roundLabel->setDefaultTextColor(QColor(80,80,0,200));


    QRectF viewRect = m_view->mapToScene(m_view->viewport()->rect())
                          .boundingRect();
    QPointF center = viewRect.center();

    QRectF tb = m_roundLabel->boundingRect();
    m_roundLabel->setPos(
        center.x() - tb.width()*0.5,
        center.y() - tb.height()*0.5
        );

    m_roundLabel->setVisible(true);
    m_roundLabelTimer->start(2000);
}

void mentevacia::spawnScaledEnemies(int round) {
  for (auto* e: m_enemigos) { m_scene->removeItem(e); delete e; }
  m_enemigos.clear();

  qreal hFactor = 1.0f + round * 0.10f;
  qreal dFactor = 1.0f + round * 0.05f;

  int num = 2 + round;
  m_initialEnemies = num;

  for (int i = 0; i < num; ++i) {
    Enemigo* e = nullptr;
    if (i == 0 && round % 5 == 0) {
      auto* boss = new Minotaur(this);
      e = boss;
      m_boss = boss;
    } else {
      e = (i % 2) ? static_cast<Enemigo*>(new Skeleton(this))
                  : static_cast<Enemigo*>(new MonsterFly(this));
    }

    int baseHP = e->maxHP();
    e->revive(int(baseHP * hFactor));
    if (auto* sk = dynamic_cast<Skeleton*>(e)) {
      sk->setDamage(int(sk->damage() * dFactor));
    } else if (auto* mf = dynamic_cast<MonsterFly*>(e)) {
      mf->setDamage(int(mf->damage() * dFactor));
    } else if (auto* boss = dynamic_cast<Minotaur*>(e)) {
      boss->setDamage(int(boss->damage() * dFactor));
    }

    qreal x = 200 + i * 300;
    qreal y = m_bgHeight - 100;
    e->setPos(x, y);
    e->setTarget(m_player);
    m_scene->addItem(e);
    m_enemigos.append(e);
  }
}
void mentevacia::updateInfoText()
{
    int creados   = m_initialEnemies;
    int restantes = std::count_if(
        m_enemigos.begin(), m_enemigos.end(),
        [](Enemigo* e){ return e && !e->isDead(); }
        );

    m_infoText->setPlainText(
        QStringLiteral("Ronda %1    Creados: %2    Restan: %3")
            .arg(m_currentRound)
            .arg(creados)
            .arg(restantes)
        );

    QPointF tl = m_view->mapToScene(0,0);
    m_infoText->setPos(
        tl.x() + HUD_MARGIN,
        tl.y() + HUD_MARGIN + HUD_H + 4
            + HUD_H + 4
        );
}
void mentevacia::keyPressEvent(QKeyEvent* e)
{
    if (m_deathScheduled) return;

    switch (e->key()) {
    case Qt::Key_A:     m_moveLeft  = true;  break;
    case Qt::Key_D:     m_moveRight = true;  break;
    case Qt::Key_Shift: m_run       = true;  break;
    case Qt::Key_Q:
        if (m_player->Getmana() >= 10) {
            m_player->Setmana(m_player->Getmana()-10);
            SpriteState st = (m_player->getLastDirection() == SpriteState::WalkingLeft ||
                              m_player->getLastDirection() == SpriteState::RunningLeft)
                                 ? SpriteState::throwingLeft
                                 : SpriteState::throwing;
            m_player->reproducirAnimacionTemporal(st, 0.7f);
            lanzarHechizo();
        }
        break;
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
void mentevacia::lanzarHechizo()
{
    bool izq = (m_player->getLastDirection() == SpriteState::WalkingLeft ||
                m_player->getLastDirection() == SpriteState::RunningLeft);
    QPointF inicio = m_player->transform().getPosition();
    auto* fb = new Fireball(izq, inicio, m_scene, &m_enemigos);
    m_fireballs.append(fb);
}


void mentevacia::keyReleaseEvent(QKeyEvent* e)
{
    if(m_deathScheduled) return;
    switch(e->key()){
    case Qt::Key_A:     m_moveLeft  = false; break;
    case Qt::Key_D:     m_moveRight = false; break;
    case Qt::Key_Shift: m_run       = false; break;
    default: QWidget::keyReleaseEvent(e);
    }
}
void mentevacia::mousePressEvent(QMouseEvent*)
{
    if(!m_player) return;
    SpriteState st = (m_player->getLastDirection()==SpriteState::WalkingLeft ||
                      m_player->getLastDirection()==SpriteState::RunningLeft)
                         ? SpriteState::SlashingLeft
                         : SpriteState::Slashing;
    m_player->reproducirAnimacionTemporal(st,0.6f);
}

void mentevacia::onFrame()
{
    if (!m_player) return;
    float dt = m_dt;
    QPointF footPos = m_player->transform().getPosition();
    float x = footPos.x();

    if (x < 0.0f) {
        m_timer->stop();
        m_mainWindow->cargarNivel("TorreDeLaMarca");
        return;
    }


    if (x >= 6245.67f && bossDefeated) {
        m_timer->stop();
        m_mainWindow->cargarNivel("MaquinaDelOlvido");
        return;
    }


    if (!m_deathScheduled && m_player->currentHP() <= 0) {
        auto* jug = dynamic_cast<Jugador*>(m_player);

        m_player->fisica().setVelocity(0,0);
        jug->setOnGround(true);
        jug->reproducirAnimacionTemporal(SpriteState::dead, 1.5f);
        m_deathScheduled = true;


        QTimer::singleShot(1000, this, [this]() {
            m_playerItem->setVisible(false);
        });
        QTimer::singleShot(2000, this, [this, jug]() {

            m_player->transform().setPosition(m_spawnPos.x(), m_spawnPos.y());
            jug->setOnGround(true);
            jug->sprite().setState(SpriteState::Idle);
            jug->setHP(jug->maxHP());
            jug->Setmana(jug->maxMana());
            m_playerItem->setVisible(true);


            qDeleteAll(m_enemigos);
            m_enemigos.clear();
            m_enemySpawnPos.clear();
            qDeleteAll(m_drops);
            m_drops.clear();
            m_deadDrops.clear();


            m_currentRound = 0;
            startNextRound();


            m_moveLeft = m_moveRight = m_run = m_jumpRequested = false;
            m_deathScheduled = false;
        });
    }


    int spawned = m_enemigos.size() + m_deadDrops.size();
    int remaining = m_enemigos.size();
    QString info = QString("Ronda %1    Creados: %2    Restan: %3")
                       .arg(m_currentRound)
                       .arg(spawned)
                       .arg(remaining);
    m_infoText->setPlainText(info);

    int restantes = std::count_if(
        m_enemigos.begin(), m_enemigos.end(),
        [](Enemigo* e){ return e && !e->isDead(); }
        );
    if (m_roundActive && restantes == 0) {
        m_roundActive = false;
        startNextRound();
        return;
    }

    if (m_jumpRequested && m_player->isOnGround()) {
        constexpr float JUMP_VY = -500.0f;
        auto v = m_player->fisica().velocity();
        m_player->fisica().setVelocity(v.x(), JUMP_VY);
        m_player->setOnGround(false);
        m_jumpRequested = false;
    }
    float vx = 0;
    if (!m_deathScheduled) {
        if (m_moveLeft)  vx = -160.0f;
        if (m_moveRight) vx =  160.0f;
        if (m_run && vx) vx *= 2.0f;
    }
    m_player->fisica().setVelocity(vx, m_player->fisica().velocity().y());


    m_player->actualizar(dt);
    QSize sprSz = m_player->sprite().getSize();
    m_colManager->resolveCollisions(m_player, sprSz, dt);



    for (auto* e : m_enemigos) {
        e->update(dt);
        QSize eSz = e->pixmap().size();
        m_colManager->resolveCollisions(e, eSz, dt);
    }


    if (!m_enemigos.isEmpty()) {
        if (m_boss && !bossDefeated && m_boss->isDead()) {
            bossDefeated = true;
            if (!m_bossDropCreado) {
                m_bossDropCreado = true;
                QPointF p = m_boss->pos();
                m_drops.append(new Drop(Drop::Tipo::Vida,  p+QPointF(-10,0), m_scene));
                m_drops.append(new Drop(Drop::Tipo::Mana,  p+QPointF( 10,0), m_scene));
                m_drops.append(new Drop(Drop::Tipo::Llave, p+QPointF(  0,-20), m_scene, "Mente Vacia"));
            }

        }
        for (auto* e : m_enemigos) {
            if (e != m_boss && e->isDead() && !m_deadDrops.contains(e)) {
                m_deadDrops.insert(e);
                QRectF sb = e->sceneBoundingRect();
                m_drops.append(new Drop(
                    Drop::Tipo::Vida,
                    QPointF(sb.left(), sb.bottom()),
                    m_scene
                    ));
            }
        }
    }

    if (!m_boss || bossDefeated) {
        m_bossHpBorder   ->setVisible(false);
        m_bossHpBar      ->setVisible(false);
        m_debugBossHitbox->setVisible(false);
    } else {
        QRectF sb = m_boss->sceneBoundingRect();
        m_debugBossHitbox->setRect(0,0,sb.width(),sb.height());
        m_debugBossHitbox->setPos(sb.topLeft());

        float frac = float(m_boss->currentHP())/m_boss->maxHP();
        float bw   = m_bossHpBorder->rect().width();
        float bh   = m_bossHpBorder->rect().height();
        float x0   = sb.left() + (sb.width()-bw)/2.0f;
        float y0   = sb.top()  - bh - 4.0f;

        m_bossHpBorder->setRect(0,0,bw,bh);
        m_bossHpBorder->setPos(x0,y0);
        m_bossHpBar   ->setRect(1,1,(bw-2)*frac,bh-2);
        m_bossHpBar   ->setPos(x0,y0);
    }
    if (m_combate) m_combate->update(dt);
    QPixmap pix = trimBottom(
        m_player->sprite().currentFrame()
            .scaled(sprSz, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    m_playerItem->setPixmap(pix);
    m_playerItem->setOffset(-pix.width()/2.0, -pix.height());
    m_playerItem->setPos(footPos);
    m_view->centerOn(footPos);
    if (m_roundLabel->isVisible()) {
        QPointF tl = m_view->mapToScene(0, 0);
        QRectF tb = m_roundLabel->boundingRect();
        qreal x = tl.x() + WINDOW_W*0.5f - tb.width()*0.5f;
        qreal y = tl.y() + WINDOW_H*0.5f - tb.height()*0.5f;
        m_roundLabel->setPos(x, y);
    }
    QPointF tl = m_view->mapToScene(0,0);
    float hpFrac = float(m_player->currentHP())/m_player->maxHP();
    m_hudBorder->setPos(tl.x()+HUD_MARGIN, tl.y()+HUD_MARGIN);
    m_hudBar->setRect(tl.x()+HUD_MARGIN+1, tl.y()+HUD_MARGIN+1,
                      (HUD_W-2)*hpFrac, HUD_H-2);
    int hpPct = int(hpFrac*100 + .5f);
    m_hudText->setPlainText(QString::number(hpPct) + "%");
    m_hudText->setPos(
        tl.x()+HUD_MARGIN + (HUD_W-m_hudText->boundingRect().width())/2,
        tl.y()+HUD_MARGIN + (HUD_H-m_hudText->boundingRect().height())/2
        );

    float manaFrac = float(m_player->Getmana())/m_player->maxMana();
    int manaPct = qMin(100, int(manaFrac*100 + .5f));
    m_hudManaBorder->setPos(tl.x()+HUD_MARGIN, tl.y()+HUD_MARGIN+HUD_H+4);
    m_hudManaBar->setRect(tl.x()+HUD_MARGIN+1, tl.y()+HUD_MARGIN+HUD_H+5,
                          (HUD_W-2)*manaFrac, HUD_H-2);
    m_manaText->setPlainText(QString::number(manaPct) + "%");
    m_manaText->setPos(
        tl.x()+HUD_MARGIN + (HUD_W-m_manaText->boundingRect().width())/2,
        tl.y()+HUD_MARGIN+HUD_H+5 + (HUD_H-m_manaText->boundingRect().height())/2
        );

    qreal x0 = tl.x() + HUD_MARGIN;
    qreal y0 = tl.y() + HUD_MARGIN + HUD_H + 4
               + HUD_H + 4;
    m_infoText->setPos(x0, y0);
    updateInfoText();

    for (int i = m_fireballs.size()-1; i >= 0; --i) {
        Fireball* f = m_fireballs[i];
        if (!f || !f->isAlive()) {
            m_fireballs.remove(i);
        } else {
            f->avanzar(dt);
        }
    }

    for (int i = m_drops.size()-1; i >= 0; --i) {
        Drop* d = m_drops[i];
        if (!d->isCollected() && d->checkCollision(m_player))
            d->aplicarEfecto(dynamic_cast<Jugador*>(m_player));
    }



}


