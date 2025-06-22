#include "niveltorredelamarca.h"
#include "mapawidget.h"
#include "ObjetosYColisiones.h"
#include "jugador.h"
#include "Enemigo.h"
#include "combatemanager.h"
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
#include "Minotaur.h"
#include "monsterfly.h"
#include "MutantWorm.h"


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
void niveltorredelamarca::penalizarCanones() {
    for (Cannon* c : m_cannons) {
        // divide por 2 el intervalo de disparo
        c->setFireRate( c->fireRate() * 0.5f );
    }
}

void niveltorredelamarca::rewardPlayerExtraDamage() {
    auto* jug = dynamic_cast<Jugador*>(m_player);
    if (!jug) return;
    jug->setDamageMultiplier( jug->damageMultiplier() * 1.05f );
}
// =========================================================
niveltorredelamarca::niveltorredelamarca(entidad*   jugador,
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
    QPixmap bgOrig(":/resources/Torre_De_La_Marca.png");
    QPixmap bg = bgOrig.scaled(bgOrig.size()*0.9,
                               Qt::KeepAspectRatioByExpanding,
                               Qt::SmoothTransformation);
    m_bgWidth  = bg.width();
    m_bgHeight = bg.height();
    float groundY = m_bgHeight;       // o m_bgHeight-40 si quieres justo encima del suelo
    float skyY    = 0;
    m_scene->setSceneRect(0,0,m_bgWidth*2, m_bgHeight);
    for (int i=0;i<2;++i){
        auto* item = m_scene->addPixmap(bg);
        item->setZValue(0);  item->setPos(i*m_bgWidth,0);
    }
    QPixmap bg2Orig(":/resources/Torre_De_La_Marca2.png");
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

    static constexpr float PLAT_W = 200.0f;
    static constexpr float PLAT_H = 400.0f;

    const QVector<QRectF> plataformas = {
        // — Bloque 1 —
        {  600.0f, 550.0f, PLAT_W, PLAT_H },
        { 1000.0f, 500.0f, PLAT_W, PLAT_H },
        { 1400.0f, 450.0f, PLAT_W, PLAT_H },
        { 1800.0f, 520.0f, PLAT_W, PLAT_H },
        { 2200.0f, 430.0f, PLAT_W, PLAT_H },
        { 2600.0f, 530.0f, PLAT_W, PLAT_H },
        { 3000.0f, 480.0f, PLAT_W, PLAT_H },

        // — Bloque 2 —
        { 3400.0f, 550.0f, PLAT_W, PLAT_H },
        { 3800.0f, 500.0f, PLAT_W, PLAT_H },
        { 4200.0f, 450.0f, PLAT_W, PLAT_H },
        { 4600.0f, 520.0f, PLAT_W, PLAT_H }
    };


    QPixmap lavaBrick(":/resources/plataforma.png");
    // en el constructor o init:
    for (const QRectF& r : plataformas) {
        // 1) sprite
        QPixmap px = lavaBrick.scaled(
            int(r.width()), int(r.height()),
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation
            );
        auto* vis = new QGraphicsPixmapItem(px);
        vis->setPos(r.topLeft());
        vis->setZValue(1);
        m_scene->addItem(vis);

        // 2) hitbox+registro
        auto* hit = m_colManager->addRect(r, Qt::NoBrush, /*collisionOnly=*/true);

        // 3) guardar en tu vector
        MovingPlatform mp;
        mp.sprite = vis;
        mp.hitbox = hit;
        mp.minX   = r.x() - 100;
        mp.maxX   = r.x() + 100;
        mp.speed  = 80.0f;
        mp.dir    = +1;
        m_movingPlatforms.append(mp);
    }

    float startX = 299.0f;
    float endX   = 3592.33f;
    float minY   = 200.0f;
    float maxY   = m_bgHeight - 40.0f - 100.0f;
    int   count  = 15;
    float minGapX = 50.0f;
    float minGapY = 40.0f;


    m_colManager->addRect({0.0f, m_bgHeight-40.0f,float(m_bgWidth*2),40.0f}, Qt::NoBrush, true);

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


    // en niveltorredelamarca.cpp, constructor:

    float W = m_scene->sceneRect().width();
    Jugador* jug         = dynamic_cast<Jugador*>(m_player);

    // Define 8 posiciones X equiespaciadas entre el 10% y el 90% de W
    QVector<qreal> xs = {
        W * 0.10f,
        W * 0.20f,
        W * 0.30f,
        W * 0.40f,
        W * 0.50f,
        W * 0.60f,
        W * 0.70f,
        W * 0.80f
    };
    m_cannons.append(new Cannon(dynamic_cast<Jugador*>(m_player), m_scene, W*0.25f, Cannon::Top));
    // Crea los 8 cañones siempre “Top”
    for (qreal x : xs) {
        m_cannons.append(
            new Cannon(
                jug,
                m_scene,
                x,
                Cannon::Top
                )
            );
    }



    Skeleton* sk = new Skeleton(this);
    QPointF skPos = {1000, 500};
    sk->setPos(skPos);
    sk->setTarget(m_player);
    m_scene->addItem(sk);
    m_enemigos.append(sk);
    m_enemySpawnPos.append(sk->pos());


    // **Añade también su spawnPos**:
    m_enemySpawnPos.append(skPos);
    // Fly

    //minotaur
    auto* boss = new Minotaur(this);
    boss->setPos(4500, 520);      // coordenadas de aparición
    boss->setTarget(m_player);
    m_scene->addItem(boss);
    m_enemigos.append(boss);
    m_boss = boss;
     m_enemySpawnPos.append(boss->pos());


    QPointF flyPos{1800.0f, 520.0f};
    auto* fly = new MonsterFly(this);
    fly->setPos(flyPos);
    fly->setTarget(m_player);
    m_scene->addItem(fly);
    m_enemigos.append(fly);
    m_enemySpawnPos.append(fly->pos());


    m_enemigos.append(fly);
    m_enemySpawnPos.append(flyPos);


    // Worm

    QPointF wormPos{3000.0f, 480.0f};
    auto* worm = new MutantWorm(this);
    worm->setPos(wormPos);
    worm->setTarget(m_player);
    m_scene->addItem(worm);


    m_enemigos.append(worm);
    m_enemySpawnPos.append(wormPos);



    m_enemigos.append(worm);
    m_enemySpawnPos.append(worm->pos());



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

    float npcX = 200.0f;
    m_npc = new NPC(this,dynamic_cast<Jugador*>(m_player), m_scene,  QPointF(npcX, m_bgHeight - 40.0f), this);

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

    connect(m_timer,&QTimer::timeout,this,&niveltorredelamarca::onFrame);
    m_timer->start(int(m_dt*1000));
}

/* =========================================================//---------------------------------------------------------------------------------------
 *  Entrada (no cambia)
 * ========================================================= */
void niveltorredelamarca::keyPressEvent(QKeyEvent* e)
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
            lanzarHechizo();  // función que haremos abajo
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
void niveltorredelamarca::lanzarHechizo()
{
    bool izq = (m_player->getLastDirection() == SpriteState::WalkingLeft ||
                m_player->getLastDirection() == SpriteState::RunningLeft);
    QPointF inicio = m_player->transform().getPosition();
    auto* fb = new Fireball(izq, inicio, m_scene, &m_enemigos);
    m_fireballs.append(fb);
}


void niveltorredelamarca::keyReleaseEvent(QKeyEvent* e)
{
    if(m_deathScheduled) return;
    switch(e->key()){
    case Qt::Key_A:     m_moveLeft  = false; break;
    case Qt::Key_D:     m_moveRight = false; break;
    case Qt::Key_Shift: m_run       = false; break;
    default: QWidget::keyReleaseEvent(e);
    }
}
void niveltorredelamarca::mousePressEvent(QMouseEvent*)
{
    if(!m_player) return;
    SpriteState st = (m_player->getLastDirection()==SpriteState::WalkingLeft ||
                      m_player->getLastDirection()==SpriteState::RunningLeft)
                         ? SpriteState::SlashingLeft
                         : SpriteState::Slashing;
    m_player->reproducirAnimacionTemporal(st,0.6f);
}

/* =========================================================
 *  Loop principal
 * ========================================================= */
void niveltorredelamarca::onFrame()
{
    if (!m_player) return;
    float dt = m_dt;
    QPointF footPos = m_player->transform().getPosition();
    float x = footPos.x();

    // 1) Salir por la izquierda → tutorial
    if (x < 0.0f) {
        m_timer->stop();
        m_mainWindow->cargarNivel("RaicesOlvidadas");
        return;
    }

    // 2) Avanzar a la Ciudad Vacia si el boss ya fue derrotado
    if (x >= 6245.67f && bossDefeated) {
        m_timer->stop();
        m_mainWindow->cargarNivel("MenteVacia");
        return;
    }

    // 3) Muerte del jugador → animación + respawn completo
    if (!m_deathScheduled && m_player->currentHP() <= 0) {
        auto* jug = dynamic_cast<Jugador*>(m_player);
        // Detener física, animar muerte
        m_player->fisica().setVelocity(0,0);
        jug->setOnGround(true);
        jug->reproducirAnimacionTemporal(SpriteState::dead, 1.5f);
        m_deathScheduled = true;

        // Después de 1s, ocultar al jugador
        QTimer::singleShot(1000, this, [this]() {
            m_playerItem->setVisible(false);
        });
        // Después de 2s, respawnear jugador + enemigos
        QTimer::singleShot(2000, this, [this, jug]() {
            // — Respawn jugador —
            m_player->transform().setPosition(m_spawnPos.x(),m_spawnPos.y());
            jug->setOnGround(true);
            jug->sprite().setState(SpriteState::Idle);
            jug->setHP(jug->maxHP());
            jug->Setmana(jug->maxMana());
            m_playerItem->setVisible(true);

            // — Respawn ENEMIGOS —
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

            // — Reset controles y bandera —
            m_moveLeft = m_moveRight = m_run = m_jumpRequested = false;
            m_deathScheduled = false;
            for (int i = 0; i < m_enemigos.size(); ++i) {
                Enemigo* e = m_enemigos[i];
                e->setPos( m_enemySpawnPos[i] );
                e->setHP( e->maxHP() );
                e->setEstado(Enemigo::Estado::Idle);
                e->setVisible(true);           // ← asegúrate que vuelvan a verse
                // si tienes método para “alive”:
                // e->setAlive(true);
            }
            // limpia y reinicia flags…
            m_drops.clear();
            m_deadDrops.clear();
            bossDefeated     = false;
            m_bossDropCreado = false;

            // ——— restablece input del jugador ———
            m_moveLeft = m_moveRight = m_run = m_jumpRequested = false;
            m_playerItem->setVisible(true);
            m_deathScheduled = false;
        });
    }

    // 4) Salto y movimiento horizontal
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

    // 5) Actualizar jugador y colisiones
    m_player->actualizar(dt);
    QSize sprSz = m_player->sprite().getSize();
    m_colManager->resolveCollisions(m_player, sprSz, dt);

    // 6) Mostrar segundo fondo (paralaje)
    if (!m_secondBgShown &&
        m_player->transform().getPosition().x() >= (m_bgWidth - WINDOW_W/2.0f))
    {
        m_bg2Item->setVisible(true);
        m_secondBgShown = true;
    }

    // 7) Actualizar cañones y avanzar escena
    for (auto* c : m_cannons) c->update(dt);
    m_scene->advance();

    // 8) NPC
    if (m_npc) m_npc->update(dt);

    // 9) Plataformas móviles
    for (auto &mp : m_movingPlatforms) {
        float nx = mp.sprite->x() + mp.speed*dt*mp.dir;
        if (nx < mp.minX) { nx = mp.minX; mp.dir = +1; }
        if (nx > mp.maxX) { nx = mp.maxX; mp.dir = -1; }
        mp.sprite->setX(nx);
        auto hb = mp.hitbox->rect();
        mp.hitbox->setRect(nx, hb.y(), hb.width(), hb.height());
    }

    // 10) Enemigos: update + colisiones
    for (auto* e : m_enemigos) {
        e->update(dt);
        QSize eSz = e->pixmap().size();
        m_colManager->resolveCollisions(e, eSz, dt);
    }

    // 11) Drops al morir (boss + minibosses)
    if (!m_enemigos.isEmpty()) {
        // Boss
        if (!bossDefeated && m_boss->isDead()) {
            bossDefeated = true;
            if (!m_bossDropCreado) {
                m_bossDropCreado = true;
                QPointF p = m_boss->pos();
                m_drops.append(new Drop(Drop::Tipo::Vida,  p+QPointF(-10,0), m_scene));
                m_drops.append(new Drop(Drop::Tipo::Mana,  p+QPointF( 10,0), m_scene));
                m_drops.append(new Drop(Drop::Tipo::Llave, p+QPointF(  0,-20), m_scene, "Torre De La Marca"));
            }
            if (m_npc) m_npc->onBossDefeated();
        }
        // Minibosses
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

    // 12) Barra de vida del boss
    if (bossDefeated) {
        m_bossHpBorder  ->setVisible(false);
        m_bossHpBar     ->setVisible(false);
        m_debugBossHitbox->setVisible(false);
    } else {
        QRectF sb = m_boss->sceneBoundingRect();
        m_debugBossHitbox->setRect(0,0,sb.width(),sb.height());
        m_debugBossHitbox->setPos(sb.topLeft());
        float frac = float(m_boss->currentHP())/m_boss->maxHP();
        float bw = m_bossHpBorder->rect().width(),
            bh = m_bossHpBorder->rect().height();
        float x0 = sb.left() + (sb.width()-bw)/2.0f,
            y0 = sb.top()  - bh - 4.0f;
        m_bossHpBorder->setRect(0,0,bw,bh);
        m_bossHpBorder->setPos(x0,y0);
        m_bossHpBar->setRect(1,1,(bw-2)*frac,bh-2);
        m_bossHpBar->setPos(x0,y0);
    }

    // 13) Combate
    if (m_combate) m_combate->update(dt);

    // 14) Render jugador + cámara
    QPixmap pix = trimBottom(
        m_player->sprite().currentFrame()
            .scaled(sprSz, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    m_playerItem->setPixmap(pix);
    m_playerItem->setOffset(-pix.width()/2.0, -pix.height());
    m_playerItem->setPos(footPos);
    m_view->centerOn(footPos);

    // 15) HUD vida & mana
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

    // 16) Fireballs
    for (int i = m_fireballs.size()-1; i >= 0; --i) {
        Fireball* f = m_fireballs[i];
        if (!f || !f->isAlive()) {
            m_fireballs.remove(i);
        } else {
            f->avanzar(dt);
        }
    }

    // 17) Recolectar drops
    for (int i = m_drops.size()-1; i >= 0; --i) {
        Drop* d = m_drops[i];
        if (!d->isCollected() && d->checkCollision(m_player))
            d->aplicarEfecto(dynamic_cast<Jugador*>(m_player));
    }
}


