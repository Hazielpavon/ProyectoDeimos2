#include "tutorialscene.h"
#include "Minotaur.h"
#include "MutantWorm.h"
#include "Skeleton.h"
#include "mapawidget.h"
#include "ObjetosYColisiones.h"
#include "jugador.h"
#include "BringerOfDeath.h"
#include "CombateManager.h"
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
#include "monsterfly.h"
#include "npc_tutorial.h"


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
static constexpr float MINI_PLAT_WIDTH  = 110.0f;
static constexpr float MINI_PLAT_HEIGHT = 20.0f;
static constexpr float Mid_PLAT_WIDTH  = 300.0f;
static constexpr float Mid_PLAT_HEIGHT = 20.0f;

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
TutorialScene::TutorialScene(entidad*   jugador,
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
    // ---- Fondo ----
    QPixmap bgOrig(":/resources/templo.png");
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
    QPixmap bg2Orig(":/resources/templo2.png");
    if(!bg2Orig.isNull()){
        m_bg2Item = m_scene->addPixmap(
            bg2Orig.scaled(m_bgWidth, m_bgHeight,
                           Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation));
        m_bg2Item->setPos(m_bgWidth,0);
        m_bg2Item->setZValue(0);
        m_bg2Item->setVisible(false);
    }

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

    static constexpr float PLAT_W = PLAT_WIDTH;
    static constexpr float PLAT_H = PLAT_HEIGHT;


    m_colManager->addRect({0.0f, m_bgHeight-40.0f,float(m_bgWidth*2),40.0f}, Qt::NoBrush, true);



    const QVector<QRectF> plataformas = {
                                         // Plataformas grandes existentes
                                         // {  500.0f, 600.0f, PLAT_W, PLAT_H },
                                         // { 1011.0f, 250.0f, PLAT_W, PLAT_H },
                                         // { 1015.0f, 250.0f, PLAT_W, PLAT_H },
                                         // { 1020.0f, 250.0f, PLAT_W, PLAT_H },
                                         // { 1080.0f, 250.0f, PLAT_W, PLAT_H },
                                         // { 1120.0f, 250.0f, PLAT_W, PLAT_H },
                                         // { 2200.0f, 430.0f, PLAT_W, PLAT_H },
                                         // { 2600.0f, 530.0f, PLAT_W, PLAT_H },
                                         // { 3000.0f, 480.0f, PLAT_W, PLAT_H },

                                         // Plataformas pequeñas nuevas
                                         { 10.0f, 250.0f, MINI_PLAT_WIDTH, MINI_PLAT_HEIGHT },
                                         { 140.0f, 310.0f, MINI_PLAT_WIDTH, MINI_PLAT_HEIGHT },
                                         { 290.0f, 210.0f, MINI_PLAT_WIDTH, MINI_PLAT_HEIGHT },
                                         { 420.0f, 310.0f, MINI_PLAT_WIDTH, MINI_PLAT_HEIGHT },
                                         { 580.0f, 370.0f, MINI_PLAT_WIDTH, MINI_PLAT_HEIGHT },
                                         { 740.0f, 250.0f, MINI_PLAT_WIDTH, MINI_PLAT_HEIGHT },
                                         { 870.0f, 310.0f, MINI_PLAT_WIDTH, MINI_PLAT_HEIGHT },
                                         { 1010.0f, 250.0f, MINI_PLAT_WIDTH, MINI_PLAT_HEIGHT },


                                         // { 1011.0f, 250.0f, Mid_PLAT_WIDTH, Mid_PLAT_HEIGHT },
                                      { 1200.0f, 250.0f, Mid_PLAT_WIDTH, Mid_PLAT_HEIGHT },
                           };

    QPixmap lavaBrick(":/resources/plataforma_normal.png");
    if (lavaBrick.isNull()) {
        qWarning() << "No se pudo cargar la textura de plataforma de fuego!";
    }





    for (const QRectF &r : plataformas) {
        // textura visual
        if (!lavaBrick.isNull()) {
            QGraphicsPixmapItem* visual = new QGraphicsPixmapItem(
                lavaBrick.scaled(int(r.width()), int(r.height()), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
                );
            visual->setPos(r.topLeft());
            visual->setZValue(1);  // Debajo del jugador
            m_scene->addItem(visual);
        }

        // hitbox
        m_colManager->addRect(r, Qt::NoBrush, true);  // Solo colisión
    }

    QRectF platMovilRect(1500.0f, 250.0f, MINI_PLAT_WIDTH, MINI_PLAT_HEIGHT);

    QPixmap platMovilPix = lavaBrick.scaled(
        int(platMovilRect.width()), int(platMovilRect.height()),
        Qt::IgnoreAspectRatio, Qt::SmoothTransformation
        );
    auto* spriteMovil = new QGraphicsPixmapItem(platMovilPix);
    spriteMovil->setPos(platMovilRect.topLeft());
    spriteMovil->setZValue(1);
    m_scene->addItem(spriteMovil);

    // CORRECCIÓN AQUÍ
    auto* hitboxMovil = new QGraphicsRectItem();
    hitboxMovil->setRect(0, 0, platMovilRect.width(), platMovilRect.height());
    hitboxMovil->setPos(platMovilRect.topLeft());
    hitboxMovil->setBrush(Qt::NoBrush);
    hitboxMovil->setPen(QPen(Qt::red));
    hitboxMovil->setZValue(1);
    m_scene->addItem(hitboxMovil);
    // Guardar plataforma móvil
    MovingPlatform mp;
    mp.sprite = spriteMovil;
    mp.hitbox = hitboxMovil;
    mp.minY = platMovilRect.y() - 100.0f;
    mp.maxY = platMovilRect.y() + 400.0f;
    mp.speed = 80.0f;
    mp.dir = +1;
    m_movingPlatforms.append(mp);



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


    auto* boss = new BringerOfDeath(this);
    QSize bSz = boss->pixmap().size();
    boss->setPos(4072.33,651 );
    boss->setTarget(m_player);
    m_scene->addItem(boss);
    m_enemigos.append(boss);
    m_boss = boss;


    Skeleton* sk = new Skeleton(this);
    QPointF skPos = {1000, 500};
    sk->setPos(skPos);
    sk->setTarget(m_player);
    m_scene->addItem(sk);
    m_enemigos.append(sk);
    m_enemySpawnPos.append(sk->pos());


    QPointF flyPos{1800.0f, 520.0f};
    auto* fly = new MonsterFly(this);
    fly->setPos(flyPos);
    fly->setTarget(m_player);
    m_scene->addItem(fly);
    m_enemigos.append(fly);
    m_enemySpawnPos.append(fly->pos());



    // Worm

    QPointF wormPos{3000.0f, 480.0f};
    auto* worm = new MutantWorm(this);
    worm->setPos(wormPos);
    worm->setTarget(m_player);
    m_scene->addItem(worm);
    m_enemigos.append(worm);
    m_enemySpawnPos.append(wormPos);



    QPixmap img(":/resources/caminar.png");
    if (!img.isNull()) {
        QPixmap scaled = img.scaled(250, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // ajustá estos valores
        m_tutorialItem = new QGraphicsPixmapItem(scaled);
        m_tutorialItem->setZValue(99);
        m_scene->addItem(m_tutorialItem);
    }

    // debug hitbox en escena
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

    float npcX = 1200.0f + Mid_PLAT_WIDTH / 2.0f;
    float npcY = 250.0f;

    m_npc = new npc_tutorial(this, dynamic_cast<Jugador*>(m_player), m_scene, QPointF(npcX, npcY), this);

    // ---- Gestor de Combate ----
    Jugador* jugadorPtr = dynamic_cast<Jugador*>(m_player);
    if (!jugadorPtr) {
        qCritical() << "[NivelRaicesOlvidadas] m_player no es Jugador!";
    } else {
        m_combate = new CombateManager(jugadorPtr, m_enemigos, this);
    }

    // ---- Mapa + HUD (igual que antes) ----
    m_mapaRegiones = new MapaWidget("Templo del Silencio", this);
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

    connect(m_timer,&QTimer::timeout,this,&TutorialScene::onFrame);
    m_timer->start(int(m_dt*1000));
}

/* =========================================================//---------------------------------------------------------------------------------------
 *  Entrada (no cambia)
 * ========================================================= */
void TutorialScene::keyPressEvent(QKeyEvent* e)
{
    if (m_deathScheduled) return;

    switch (e->key()) {
    case Qt::Key_A:     m_moveLeft  = true;  break;
    case Qt::Key_D:     m_moveRight = true;  break;
    case Qt::Key_Shift: m_run       = true;  break;
    case Qt::Key_Q:
        if (m_player->Getmana() >= 10) {
            m_player->Setmana(m_player->Getmana() - 10);

            SpriteState st = (m_player->getLastDirection() == SpriteState::WalkingLeft ||
                              m_player->getLastDirection() == SpriteState::RunningLeft)
                                 ? SpriteState::throwingLeft
                                 : SpriteState::throwing;

            m_player->reproducirAnimacionTemporal(st, 0.7f);
            lanzarHechizo();  // ← Función ya implementada en tu juego

            // ─── Detectar FIREBALL en tutorial ───
            if (m_faseTutorial == FaseTutorial::Fireball && !m_yaFireball) {
                m_yaFireball = true;

                if (m_tutorialItem) {
                    m_scene->removeItem(m_tutorialItem);
                    delete m_tutorialItem;
                    m_tutorialItem = nullptr;
                }

                // Mostrar cartel del MAPA
                QPixmap img(":/resources/mapat.png");
                if (!img.isNull()) {
                    m_tutorialItem = new QGraphicsPixmapItem(
                        img.scaled(240, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    m_tutorialItem->setZValue(99);
                    m_scene->addItem(m_tutorialItem);
                }

                m_faseTutorial = FaseTutorial::Mapa;
                qDebug() << "[Tutorial] FIREBALL detectado. Mostrando cartel de MAPA.";
            }
        }
        break;
    case Qt::Key_Space:
        // sólo al primer evento, no auto‐repeat:
        if (!e->isAutoRepeat()) {
            m_jumpRequested = true;
        }
        break;

    case Qt::Key_M:
        m_mapaRegiones->setVisible(!m_mapaRegiones->isVisible());

        // ─── Detectar MAPA en tutorial ───
        if (m_faseTutorial == FaseTutorial::Mapa) {
            if (m_tutorialItem) {
                m_scene->removeItem(m_tutorialItem);
                delete m_tutorialItem;
                m_tutorialItem = nullptr;
            }
            m_faseTutorial = FaseTutorial::Terminado;
            qDebug() << "[Tutorial] MAPA abierto. Tutorial FINALIZADO.";
        }
        break;

    case Qt::Key_C:
        if (m_player && m_player->isOnGround()) {
            float vx = m_player->fisica().velocity().x();
            SpriteState st = vx > 0.0f ? SpriteState::Slidding : SpriteState::SliddingLeft;
            m_player->reproducirAnimacionTemporal(st, 0.5f);
        }

        if (m_faseTutorial == FaseTutorial::Dash && !m_yaDash) {
            m_yaDash = true;
        }
        break;

    default:
        QWidget::keyPressEvent(e);
    }
}
void TutorialScene::lanzarHechizo()
{
    bool izq = (m_player->getLastDirection() == SpriteState::WalkingLeft ||
                m_player->getLastDirection() == SpriteState::RunningLeft);
    QPointF inicio = m_player->transform().getPosition();
    auto* fb = new Fireball(izq, inicio, m_scene, &m_enemigos);
    m_fireballs.append(fb);
}


void TutorialScene::keyReleaseEvent(QKeyEvent* e)
{
    if(m_deathScheduled) return;
    switch(e->key()){
    case Qt::Key_A:     m_moveLeft  = false; break;
    case Qt::Key_D:     m_moveRight = false; break;
    case Qt::Key_Shift: m_run       = false; break;
    default: QWidget::keyReleaseEvent(e);
    }
}
void TutorialScene::mousePressEvent(QMouseEvent*)
{
    if (!m_player || !m_player->isOnGround()) return;

    SpriteState st = (m_player->getLastDirection() == SpriteState::WalkingLeft ||
                      m_player->getLastDirection() == SpriteState::RunningLeft)
                         ? SpriteState::SlashingLeft
                         : SpriteState::Slashing;
    m_player->reproducirAnimacionTemporal(st, 0.6f);

    // ─── Detectar GOLPE en tutorial ───
    if (m_faseTutorial == FaseTutorial::Golpear && !m_yaGolpeo) {
        m_yaGolpeo = true;

        // Eliminar cartel de golpear
        if (m_tutorialItem) {
            m_scene->removeItem(m_tutorialItem);
            delete m_tutorialItem;
            m_tutorialItem = nullptr;
        }

        // Mostrar cartel de FIREBALL
        QPixmap img(":/resources/fireball.png");
        if (!img.isNull()) {
            m_tutorialItem = new QGraphicsPixmapItem(
                img.scaled(220, 110, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_tutorialItem->setZValue(99);
            m_scene->addItem(m_tutorialItem);
        }

        m_faseTutorial = FaseTutorial::Fireball;
        qDebug() << "[Tutorial] GOLPE detectado. Mostrando FIREBALL.";
    }
}

/* =========================================================
 *  Loop principal
 * ========================================================= */
void TutorialScene::onFrame()
{
    if (!m_player) return;

    QPointF footPos1 = m_player->transform().getPosition();
    float x = footPos1.x();
    float y = footPos1.y();

    // 1) Si se sale por la izquierda, volvemos al tutorial
    if (x < 0.0f) {
        m_timer->stop();
        m_mainWindow->cargarNivel("Tutorial");
        return;
    }

    // 2) Si llega al tramo de la Torre de la Marca
    if (x >= 5467.0f && x <= 5693.67f && qFuzzyCompare(y, 651.0f) && bossDefeated) {
        m_timer->stop();
        m_mainWindow->cargarNivel("RaicesOlvidadas");
        return;
    }

    // 3) Si avanza más allá de la Ciudad Inversa
    if (x >= 6245.67f && bossDefeated ) {
        m_timer->stop();
        m_mainWindow->cargarNivel("CiudadInversa");
        return;
    }


    // ——— ZONA LETAL Y SECUENCIA DE MUERTE ———
    if (!m_deathScheduled && m_player->currentHP() <= 0) {
        auto* jug = dynamic_cast<Jugador*>(m_player);
        // 1.1) Detener movimiento y asegurar en suelo
        m_player->fisica().setVelocity(0, 0);
        jug->setOnGround(true);
        // 1.2) Lanzar animación de muerte
        jug->reproducirAnimacionTemporal(SpriteState::dead, 1.5f);
        m_deathScheduled = true;
        if (!bossDefeated && !m_enemigos.isEmpty()) {
            Enemigo* boss = m_enemigos.first();
            boss->setHP(boss->maxHP());
        }
        // 1.3) Ocultar sprite y respawn con temporizadores
        QTimer::singleShot(1000, this, [this]() { m_playerItem->setVisible(false); });
        QTimer::singleShot(2000, this, [this, jug]() {
            // reposición idéntica a tu código de respawn…
            m_player->transform().setPosition(35,0);
            m_player->fisica().setVelocity(0,0);
            jug->setOnGround(true);
            jug->sprite().setState(SpriteState::Idle);
            jug->setHP(jug->maxHP());
            jug->Setmana(jug->maxMana());

            m_moveLeft = m_moveRight = m_run = m_jumpRequested = false;
            m_playerItem->setVisible(true);
            m_deathScheduled = false;
        });
        return;  // salimos, no procesamos nada más hasta el respawn
    }

    // ——— Entrada salto + movimiento horiz ———
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

    // ─── Tutorial: CAMINAR → SALTAR → CORRER → DASH → GOLPEAR → FIREBALL ───
    if (m_faseTutorial == FaseTutorial::Caminar && (m_moveLeft || m_moveRight)) {
        if (!m_yaCamino) {
            m_yaCamino = true;
            if (m_tutorialItem) {
                m_scene->removeItem(m_tutorialItem);
                delete m_tutorialItem;
                m_tutorialItem = nullptr;
            }
            QPixmap img(":/resources/saltar.png");
            if (!img.isNull()) {
                m_tutorialItem = new QGraphicsPixmapItem(
                    img.scaled(250, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                m_tutorialItem->setZValue(99);
                m_scene->addItem(m_tutorialItem);
            }
            m_faseTutorial = FaseTutorial::Saltar;
        }
    }
    else if (m_faseTutorial == FaseTutorial::Saltar &&
             !m_yaSalto &&
             !m_player->isOnGround() &&
             m_player->fisica().velocity().y() < 0.0f)
    {
        m_yaSalto = true;
        if (m_tutorialItem) {
            m_scene->removeItem(m_tutorialItem);
            delete m_tutorialItem;
            m_tutorialItem = nullptr;
        }
        qDebug() << "[Tutorial] SALTO detectado. Esperando coordenada para mostrar CORRER...";
    }
    else if (m_faseTutorial == FaseTutorial::Saltar &&
             m_yaSalto &&
             m_player->transform().getPosition().x() > 1200.0f &&
             !m_yaCorrio)
    {
        m_yaCorrio = true;
        QPixmap img(":/resources/correr.png");
        if (!img.isNull()) {
            m_tutorialItem = new QGraphicsPixmapItem(
                img.scaled(230, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_tutorialItem->setZValue(99);
            m_scene->addItem(m_tutorialItem);
        }
        m_faseTutorial = FaseTutorial::Correr;
        qDebug() << "[Tutorial] Mostrando cartel de CORRER tras pasar por X=1200.";
    }
    else if (m_faseTutorial == FaseTutorial::Correr &&
             (m_run && (m_moveLeft || m_moveRight)))
    {
        if (m_tutorialItem) {
            m_scene->removeItem(m_tutorialItem);
            delete m_tutorialItem;
            m_tutorialItem = nullptr;
        }
        m_faseTutorial = FaseTutorial::Dash;
        qDebug() << "[Tutorial] CORRER detectado. Mostrando DASH...";
    }
    else if (m_faseTutorial == FaseTutorial::Dash && !m_yaDash)
    {
        m_yaDash = true;

        // Mostrar imagen de DASH
        QPixmap imgDash(":/resources/dash.png");
        if (!imgDash.isNull()) {
            m_tutorialItem = new QGraphicsPixmapItem(
                imgDash.scaled(220, 110, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_tutorialItem->setZValue(99);
            m_scene->addItem(m_tutorialItem);
        }

        // Pasar a la fase de GOLPEAR
        m_faseTutorial = FaseTutorial::Golpear;
        qDebug() << "[Tutorial] Mostrando cartel de GOLPEAR...";

        // Mostrar imagen de GOLPEAR inmediatamente
        QPixmap imgGolpear(":/resources/golpear.png");
        if (!imgGolpear.isNull()) {
            m_tutorialItem = new QGraphicsPixmapItem(
                imgGolpear.scaled(230, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_tutorialItem->setZValue(99);
            m_scene->addItem(m_tutorialItem);
        }
    }
    else if (m_faseTutorial == FaseTutorial::Golpear && m_yaGolpeo)
    {
        // Ocultar cartel de golpear
        if (m_tutorialItem) {
            m_scene->removeItem(m_tutorialItem);
            delete m_tutorialItem;
            m_tutorialItem = nullptr;
        }

        // Mostrar cartel de FIREBALL
        QPixmap img(":/resources/fireball.png");
        if (!img.isNull()) {
            m_tutorialItem = new QGraphicsPixmapItem(
                img.scaled(220, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_tutorialItem->setZValue(99);
            m_scene->addItem(m_tutorialItem);
        }
        m_faseTutorial = FaseTutorial::Fireball;
        qDebug() << "[Tutorial] Mostrando cartel de FIREBALL...";
    }
    else if (m_faseTutorial == FaseTutorial::Fireball && m_yaFireball)
    {
        if (m_tutorialItem) {
            m_scene->removeItem(m_tutorialItem);
            delete m_tutorialItem;
            m_tutorialItem = nullptr;
        }
        m_faseTutorial = FaseTutorial::Terminado;
        qDebug() << "[Tutorial] FIREBALL detectado. Tutorial COMPLETADO.";
    }

    else if (m_faseTutorial == FaseTutorial::Fireball && m_yaFireball)
    {
        if (m_tutorialItem) {
            m_scene->removeItem(m_tutorialItem);
            delete m_tutorialItem;
            m_tutorialItem = nullptr;
        }

        // Mostrar cartel del MAPA
        QPixmap img(":/resources/mapat.png");
        if (!img.isNull()) {
            m_tutorialItem = new QGraphicsPixmapItem(
                img.scaled(240, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_tutorialItem->setZValue(99);
            m_scene->addItem(m_tutorialItem);
        }

        m_faseTutorial = FaseTutorial::Mapa;
        qDebug() << "[Tutorial] FIREBALL detectado. Mostrando cartel de MAPA.";
    }

    // ─── Zona letal (lava) ───
    if (!m_deathScheduled) {
        if (auto* jug = dynamic_cast<Jugador*>(m_player)) {
            QPointF footPos = m_player->transform().getPosition();
            float x = footPos.x();
            float y = footPos.y();
            constexpr float epsY = 1.0f;
            bool atY      = (y >= 651.0f - epsY && y <= 651.0f + epsY);
            bool inZone   = (x >= 0.0f && x <= 1200.0f);

            if (atY && inZone && jug->currentHP() > 0) {
                // 1) Detener movimiento y asegurar en suelo
                m_player->fisica().setVelocity(0, 0);
                jug->setOnGround(true);

                // 2) Aplicar daño total y animación
                jug->aplicarDano(jug->currentHP());
                jug->reproducirAnimacionTemporal(SpriteState::dead, 1.5f);
                m_deathScheduled = true;
                for (int i = 0; i < m_enemigos.size(); ++i) {
                    Enemigo* e = m_enemigos[i];
                    e->revive(m_enemigos[i]->maxHP());       // o bien m_enemigos[i]->maxHP()
                    e->setPos(m_enemySpawnPos[i]);
                }
                bossDefeated     = false;
                m_bossDropCreado = false;

                // — Eliminar y destruir todos los drops viejos —
                // — destruir los drops todavía vivos en escena —
                for (Drop* d : m_drops)      delete d;

                // — vaciar ambos contenedores —
                m_drops.clear();
                m_deadDrops.clear();
                // 3) Ocultar sprite después de 1s
                QTimer::singleShot(1000, this, [this]() {
                    m_playerItem->setVisible(false);
                });

                // 4) Respawn a los 2s
                QTimer::singleShot(2000, this, [this, jug]() {
                    m_player->transform().setPosition(35, 0);
                    m_player->fisica().setVelocity(0, 0);
                    jug->setOnGround(true);
                    jug->sprite().setState(SpriteState::Idle);
                    jug->setHP(jug->maxHP());
                    jug->Setmana(jug->maxMana());

                    m_moveLeft = m_moveRight = m_run = m_jumpRequested = false;
                    m_playerItem->setVisible(true);
                    m_deathScheduled = false;
                });

                return;
            }
        }
    }

    // ——— Actualizar jugador + colisiones ———
    m_player->actualizar(m_dt);
    QSize sprSz = m_player->sprite().getSize();
    m_colManager->resolveCollisions(m_player, sprSz, m_dt);

    // ——— Mostrar segundo fondo si tocsa ———
    if (!m_secondBgShown &&
        m_player->transform().getPosition().x() >= (m_bgWidth - WINDOW_W/2.0f))
    {
        m_bg2Item->setVisible(true);
        m_secondBgShown = true;
    }

    // ——— Actualizar enemigos ———
    for (Enemigo* e : std::as_const(m_enemigos)) {
        e->update(m_dt);
        QSize eSz = e->pixmap().size();
        m_colManager->resolveCollisions(e, eSz, m_dt);
    }
    if (!m_enemigos.isEmpty()) {
        Enemigo* boss = m_enemigos.first();
        if (!bossDefeated && boss->isDead()) {
            bossDefeated = true;

            if (!m_bossDropCreado) {
                m_bossDropCreado = true;
                QPointF posDrop = boss->pos();  // <- ahora usamos la posición exacta del boss
                m_drops.append(new Drop(Drop::Tipo::Vida, posDrop + QPointF(-10, 0), m_scene));
                m_drops.append(new Drop(Drop::Tipo::Mana, posDrop + QPointF(10, 0), m_scene));
                m_drops.append(new Drop(Drop::Tipo::Llave, posDrop + QPointF(0, -20), m_scene, "Raices Olvidadas"));
            }

        }
        for (auto* e : m_enemigos) {
            if (e != m_boss && e->isDead() && !m_deadDrops.contains(e)) {
                m_deadDrops.insert(e);
                QRectF sb = e->sceneBoundingRect();
                m_drops.append(new Drop(Drop::Tipo::Vida,
                                        QPointF(sb.left(), sb.bottom()),
                                        m_scene));
            }
        }
    }

    // ——— Barra de vida del boss ———
    if (!m_enemigos.isEmpty() && m_boss) {
        if (m_boss->isDead()) {
            m_bossHpBorder->setVisible(false);
            m_bossHpBar   ->setVisible(false);
            m_debugBossHitbox->setVisible(false);
        } else {
            // Obtén el rectángulo del boss
            QRectF sb = m_boss->sceneBoundingRect();

            // (Opcional) Debug hitbox
            m_debugBossHitbox->setRect(0,0,sb.width(), sb.height());
            m_debugBossHitbox->setPos(sb.topLeft());

            // Cálculo del porcentaje
            float frac = float(m_boss->currentHP()) / m_boss->maxHP();

            // Anchura y altura fijas de la barra
            float bw = m_bossHpBorder->rect().width();
            float bh = m_bossHpBorder->rect().height();

            // Posiciona el borde centrado sobre el boss
            float x0 = sb.left() + (sb.width() - bw) / 2.0f;
            float y0 = sb.top()  - bh - 4.0f;

            m_bossHpBorder->setRect(0, 0, bw, bh);
            m_bossHpBorder->setPos(x0, y0);

            // Redimensiona la barra interior según frac
            m_bossHpBar->setRect(1, 1, (bw - 2) * frac, bh - 2);
            m_bossHpBar->setPos(x0, y0);
        }
    }


    // ——— Combate ———
    if (m_combate) m_combate->update(m_dt);

    // ——— Render jugador + cámara ———
    QPixmap pix = trimBottom(
        m_player->sprite().currentFrame()
            .scaled(sprSz, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    QPointF footPos = m_player->transform().getPosition();
    m_playerItem->setPixmap(pix);
    m_playerItem->setOffset(-pix.width()/2.0, -pix.height());
    m_playerItem->setPos(footPos);
    m_view->centerOn(footPos);

    // Posicionar imagen del tutorial encima del jugador
    if (m_tutorialItem) {
        QPointF arribaJugador = footPos + QPointF(-40, -180);  // 📌 más alto y centrado
        m_tutorialItem->setPos(arribaJugador);
    }

    // ——— HUD ———
    QPointF tl = m_view->mapToScene(0,0);
    float hpFrac = float(m_player->currentHP()) / m_player->maxHP();
    m_hudBorder->setPos(tl.x()+HUD_MARGIN, tl.y()+HUD_MARGIN);
    m_hudBar->setRect(
        tl.x()+HUD_MARGIN+1, tl.y()+HUD_MARGIN+1,
        (HUD_W-2)*hpFrac, HUD_H-2
        );
    int pct = int(hpFrac*100.0f + 0.5f);
    m_hudText->setPlainText(QString::number(pct) + "%");
    QRectF rt = m_hudText->boundingRect();
    m_hudText->setPos(
        tl.x()+HUD_MARGIN + (HUD_W-rt.width())/2.0f,
        tl.y()+HUD_MARGIN + (HUD_H-rt.height())/2.0f
        );
    float manaFrac = float(m_player->Getmana()) / m_player->maxMana();
    int manaPct = int(manaFrac * 100.0f + 0.5f);
    if (manaPct > 100) manaPct = 100;
    m_hudManaBorder->setPos(tl.x() + HUD_MARGIN, tl.y() + HUD_MARGIN + HUD_H + 4);
    m_hudManaBar->setRect(
        tl.x() + HUD_MARGIN + 1,
        tl.y() + HUD_MARGIN + HUD_H + 5,
        (HUD_W - 2) * manaFrac,
        HUD_H - 2
        );
    m_manaText->setPlainText(QString::number(manaPct) + "%");
    QRectF rt2 = m_manaText->boundingRect();
    m_manaText->setPos(
        tl.x() + HUD_MARGIN + (HUD_W - rt2.width()) / 2.0f,
        tl.y() + HUD_MARGIN + HUD_H + 5 + (HUD_H - rt2.height()) / 2.0f
        );
    for (int i = m_fireballs.size() - 1; i >= 0; --i) {
        Fireball* f = m_fireballs[i];
        if (!f || !f->isAlive()) {
            m_fireballs.remove(i);
        } else {
            f->avanzar(m_dt);
        }
    }
        if (m_npc) m_npc->update(m_dt);

    for (int i = m_drops.size() - 1; i >= 0; --i) {
        Drop* drop = m_drops[i];
        if (!drop->isCollected() && drop->checkCollision(m_player)) {
            drop->aplicarEfecto(dynamic_cast<Jugador*>(m_player));
        }

    }
     actualizarPlataformasMoviles();
}

void TutorialScene::actualizarPlataformasMoviles()
{
    for (MovingPlatform& mp : m_movingPlatforms) {
        float dy = mp.dir * mp.speed * m_dt;
        float newY = mp.sprite->y() + dy;

        // Rebotar al llegar a los extremos
        if (newY < mp.minY || newY > mp.maxY) {
            mp.dir *= -1;
            newY = std::clamp(newY, mp.minY, mp.maxY);
        }

        float delta = newY - mp.sprite->y();

        // Mover sprite y hitbox
        mp.sprite->setY(newY);
        mp.hitbox->moveBy(0, delta);

        // Obtener posición del pie del jugador
        QPointF foot = m_player->transform().getPosition();
        QRectF platRect = mp.sprite->sceneBoundingRect();

        // Zona superior de la plataforma (ajustada con tolerancia)
        QRectF platTop = platRect.adjusted(10, -2, -10, 6);
        QRectF footBox(foot, QSizeF(1, 1)); // Pie como punto

        // Si el jugador está sobre la plataforma y cayendo
        if (platTop.contains(footBox.center()) && m_player->fisica().velocity().y() >= 0) {
            // Colocarlo justo encima de la plataforma
            float nuevaY = platRect.top();
            m_player->transform().setPosition(foot.x(), nuevaY);
            m_player->fisica().setVelocity(m_player->fisica().velocity().x(), 0);
            m_player->setOnGround(true);
            m_playerItem->setY(nuevaY);
        }
        // Si el jugador ya está parado sobre la plataforma, moverlo con ella
        else {
            bool encima = qAbs(foot.y() - platRect.top()) <= 2.0f &&
                          foot.x() > platRect.left() + 10 &&
                          foot.x() < platRect.right() - 10;

            if (encima && m_player->isOnGround()) {
                QPointF pos = m_player->transform().getPosition();
                m_player->transform().setPosition(pos.x(), pos.y() + delta);
                m_playerItem->moveBy(0, delta);
            }
        }
    }
}
