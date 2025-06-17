#include "nivelraicesolvidadas.h"
#include "mapawidget.h"
#include "ObjetosYColisiones.h"
#include "jugador.h"
#include "Enemigo.h"
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
#include <iostream>
#include "mainwindow.h"
#include "Demon.h"
#include "Skeleton.h"
#include "Minotaur.h"
#include "monsterfly.h"
#include "MutantWorm.h"
#include "Carnivore.h"


using namespace std;
using namespace std;
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
    setFocus();
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
    float startX = 299.0f;
    float endX   = 3592.33f;
    float minY   = 200.0f;                // altura mínima
    float maxY   = m_bgHeight - 40.0f - 100.0f; // 100px por encima del suelo
    int   count  = 15;
    float minGapX = 50.0f;  // mínimo espacio horizontal entre plataformas
    float minGapY = 40.0f;  // mínimo espacio vertical entre plataformas

    // preparamos un vector para recordar las rects y poder comprobar distancias
    // tras haber inicializado m_bgWidth, m_bgHeight, etc.

    static constexpr float PLAT_W = PLAT_WIDTH;
    static constexpr float PLAT_H = PLAT_HEIGHT;


    // Suelo (collisionOnly = true)
    m_colManager->addRect({0.0f, m_bgHeight-40.0f,float(m_bgWidth*2),40.0f}, Qt::NoBrush, true);



    // Plataformas manuales, bien distribuidas entre x=[299,3592] y y=[150,450]
    const QVector<QRectF> plataformas = {
        // Δx ≈ 400, Δy ≤ 100
        {  600.0f, 550.0f, PLAT_W, PLAT_H },  // Desde suelo
        { 1000.0f, 500.0f, PLAT_W, PLAT_H },  // Δx=400, Δy=50
        { 1400.0f, 450.0f, PLAT_W, PLAT_H },  // Δx=400, Δy=50
        { 1800.0f, 520.0f, PLAT_W, PLAT_H },  // Δx=400, subida de 70
        { 2200.0f, 430.0f, PLAT_W, PLAT_H },  // Δx=400, bajada de 90
        { 2600.0f, 530.0f, PLAT_W, PLAT_H },  // Δx=400, subida de 100
        { 3000.0f, 480.0f, PLAT_W, PLAT_H }   // Δx=400, bajada de 50
    };

    for (const QRectF &r : plataformas) {
        m_colManager->addRect(r,
                              QColor(80,80,80),
                              false);
    }







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

    // ---- Enemigo (Bringer-of-Death) ----
    auto* boss = new BringerOfDeath(this);
    QSize bSz = boss->pixmap().size();
    boss->setPos(4072.33,651 );
    boss->setTarget(m_player);
    m_scene->addItem(boss);
    m_enemigos.append(boss);

    //demon
    auto* demon = new Demon(this);
    demon->setPos(2000, 651);
    demon->setTarget(m_player);
    m_scene->addItem(demon);
    m_enemigos.append(demon);

    //skeleton
    auto* sk = new Skeleton(this);
    sk->setPos(600, 651);        // posición deseada
    sk->setTarget(m_player);
    m_scene->addItem(sk);
    m_enemigos.append(sk);

    //minotaur
    auto* mina = new Minotaur(this);
    mina->setPos(400, 651);      // coordenadas de aparición
    mina->setTarget(m_player);
    m_scene->addItem(mina);
    m_enemigos.append(mina);

    //fly enemy
    auto* fly = new MonsterFly(this);
    fly->setPos(300, 450);        // un poco por encima del suelo
    fly->setTarget(m_player);
    m_scene->addItem(fly);
    m_enemigos.append(fly);

    //mutant worm
    auto* worm = new MutantWorm(this);
    worm->setPos(5200, 651);     // coordenadas iniciales
    worm->setTarget(m_player);
    m_scene->addItem(worm);
    m_enemigos.append(worm);

    //carnivore
    auto* carn = new Carnivore(this);
    carn->setPos(800, 651);   // posición inicial
    carn->setTarget(m_player);
    m_scene->addItem(carn);
    m_enemigos.append(carn);


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
    if (m_deathScheduled) return;

    switch (e->key()) {
    case Qt::Key_A:     m_moveLeft  = true;  break;
    case Qt::Key_D:     m_moveRight = true;  break;
    case Qt::Key_Shift: m_run       = true;  break;

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
        m_mainWindow->cargarNivel("TorreDeLaMarca");
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
        // 1.3) Ocultar sprite y respawn con temporizadores
        QTimer::singleShot(1000, this, [this]() { m_playerItem->setVisible(false); });
        QTimer::singleShot(2000, this, [this, jug]() {
            // reposición idéntica a tu código de respawn…
            m_player->transform().setPosition(35,0);
            m_player->fisica().setVelocity(0,0);
            jug->setOnGround(true);
            jug->sprite().setState(SpriteState::Idle);
            jug->setHP(jug->maxHP());
            m_moveLeft = m_moveRight = m_run = m_jumpRequested = false;
            m_playerItem->setVisible(true);
            m_deathScheduled = false;
        });
        return;  // salimos, no procesamos nada más hasta el respawn
    }
    if (!m_deathScheduled) {
        if (auto* jug = dynamic_cast<Jugador*>(m_player)) {
            QPointF footPos = m_player->transform().getPosition();
            float x = footPos.x();
            float y = footPos.y();
            constexpr float epsY = 1.0f;
            bool atY      = (y >= 651.0f - epsY && y <= 651.0f + epsY);
            bool inZone1  = (x >=  299.0f  && x <= 3104.33f);
            bool inZone2  = (x >= 3373.67f && x <= 3592.33f);

            if (atY && (inZone1 || inZone2) && jug->currentHP() > 0) {
                // 1) Detener movimiento y asegurar contacto con el suelo
                m_player->fisica().setVelocity(0, 0);
                jug->setOnGround(true);

                // 2) Matar al jugador y lanzar animación “dead”
                jug->aplicarDano(jug->currentHP());
                jug->reproducirAnimacionTemporal(SpriteState::dead, 1.5f);
                m_deathScheduled = true;

                // 3) Tras 1s ocultar el sprite (dejamos ver la animación un rato)
                QTimer::singleShot(1000, this, [this]() {
                    m_playerItem->setVisible(false);
                });

                // 4) Tras 2s, respawnear al jugador y resetear flags
                QTimer::singleShot(2000, this, [this, jug]() {
                    // reposicionar al spawn original
                    m_player->transform().setPosition(50,0);
                    // frenar velocidad
                    m_player->fisica().setVelocity(0, 0);
                    jug->setOnGround(true);
                    // resetear animación y vida
                    jug->sprite().setState(SpriteState::Idle);
                    jug->setHP(jug->maxHP());
                    // limpiar inputs
                    m_moveLeft = m_moveRight = m_run = m_jumpRequested = false;
                    // volver a mostrar sprite
                    m_playerItem->setVisible(true);
                    // permitir muertes futuras
                    m_deathScheduled = false;
                });

                // salimos de onFrame para no procesar nada más mientras morimos
                return;
            }
        }
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
    if (!bossDefeated && !m_enemigos.isEmpty()) {
        Enemigo* boss = m_enemigos.first();
        if (boss->isDead()) {
            bossDefeated = true;
        }
    }
    // ——— Debug hitbox y barra de vida del boss ———
    if (!m_enemigos.isEmpty()) {
        Enemigo* boss = m_enemigos.first();
        QRectF sb = boss->sceneBoundingRect();
        // hitbox
        m_debugBossHitbox->setRect(0, 0, sb.width(), sb.height());
        m_debugBossHitbox->setPos(sb.topLeft());
        // barra
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
}


