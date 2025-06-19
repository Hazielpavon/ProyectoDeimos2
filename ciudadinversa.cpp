#include "ciudadinversa.h"
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
#include "MutantWorm.h"   // 🐛 gusano mutante
#include "Minotaur.h"     // 🦬 minotauro


/* ─────────────────────────────  Constantes  ───────────────────────────── */
static constexpr float WINDOW_W    = 950.0f;
static constexpr float WINDOW_H    = 650.0f;
static constexpr float FPS         = 60.0f;
static constexpr float PLAT_WIDTH  = 200.0f;
static constexpr float PLAT_HEIGHT = 20.0f;

/* HUD */
static constexpr float HUD_W = 350.0f;
static constexpr float HUD_H = 35.0f;
static constexpr float HUD_MARGIN = 10.0f;

/* ───────── util: recorta líneas alfa vacías de la parte baja ─────────── */
static QPixmap trimBottom(const QPixmap& pix)
{
    QImage img = pix.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
    int maxY = -1;
    for (int y = 0; y < img.height(); ++y)
        for (int x = 0; x < img.width(); ++x)
            if (qAlpha(img.pixel(x, y)) > 0) maxY = std::max(maxY, y);

    return (maxY >= 0 && maxY < img.height()-1)
               ? pix.copy(0, 0, pix.width(), maxY+1)
               : pix;
}

/* ──────────────────────  ctor  ────────────────────── */
ciudadinversa::ciudadinversa(entidad*   jugador,
                             MainWindow* mainWindow,
                             QWidget*    parent)
    : QWidget(parent)
    , m_player(jugador)
    , m_mainWindow(mainWindow)
    , m_timer(new QTimer(this))
    , m_scene(new QGraphicsScene(this))
    , m_colManager(new ObjetosYColisiones(m_scene, this))
    , m_dt(1.0f / FPS)
{
    setFixedSize(int(WINDOW_W), int(WINDOW_H));
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    /* ------------------------  Fondo  ------------------------ */
    QPixmap bgOrig(":/resources/Ciudad_Inversa.png");
    QPixmap bg = bgOrig.scaled(bgOrig.size()*0.9,
                               Qt::KeepAspectRatioByExpanding,
                               Qt::SmoothTransformation);
    m_bgWidth  = bg.width();
    m_bgHeight = bg.height();
    m_scene->setSceneRect(0, 0, m_bgWidth*2, m_bgHeight);

    for (int i = 0; i < 2; ++i) {
        auto* item = m_scene->addPixmap(bg);
        item->setZValue(0);
        item->setPos(i * m_bgWidth, 0);
    }

    if (QPixmap bg2(":/resources/Ciudad_Inversa2.png"); !bg2.isNull()) {
        m_bg2Item = m_scene->addPixmap(
            bg2.scaled(m_bgWidth, m_bgHeight,
                       Qt::KeepAspectRatioByExpanding,
                       Qt::SmoothTransformation));
        m_bg2Item->setPos(m_bgWidth, 0);
        m_bg2Item->setZValue(0);
        m_bg2Item->setVisible(false);
    }

    /* ------------------------  Vista  ------------------------ */
    m_view = new QGraphicsView(m_scene, this);
    m_view->setFixedSize(int(WINDOW_W), int(WINDOW_H));
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_view->scale(1.0, -1.0);                     // ← ciudad “invertida”
    m_view->translate(0, -m_bgHeight);

    /* ----------------  Suelo y muros  ---------------- */
    constexpr float WALL = 40.f;
    m_colManager->addRect({0, 0,static_cast<qreal>(m_bgWidth*2), WALL}, Qt::NoBrush, true);          // techo
    m_colManager->addRect({0, m_bgHeight-WALL, static_cast<qreal>(m_bgWidth*2), WALL}, Qt::NoBrush, true);          // suelo
    m_colManager->addRect({0, 0,               WALL,       static_cast<qreal>(m_bgHeight)}, Qt::NoBrush, true);     // pared izq
    m_colManager->addRect({m_bgWidth*2-WALL, 0, WALL,      static_cast<qreal>(m_bgHeight)}, Qt::NoBrush, true);     // pared der

    /* ----------------  Plataformas fijas  ---------------- */
    static constexpr float PLAT_W = PLAT_WIDTH;
    static constexpr float PLAT_H = PLAT_HEIGHT;
    const float STEP = PLAT_W * .60f, GAP = 80.f;

    const QVector<QRectF> plataformasFijas = {
        { WALL+GAP,             120,  m_bgWidth*2 - 2*(WALL+GAP), PLAT_H },
        { m_bgWidth-STEP,       520,  STEP*2,                     PLAT_H },
        { m_bgWidth/2-STEP,     450,  STEP*2,                     PLAT_H },
        { m_bgWidth-STEP/2,     240,  STEP,                       PLAT_H },
        { m_bgWidth/2-STEP/2,   170,  STEP,                       PLAT_H }
    };

    for (const QRectF& r : plataformasFijas)
        m_colManager->addRect(r, QColor(80, 80, 80), false);

    /* ----------------  Plataformas móviles (2)  ---------------- */
    {
        const float MOV_W = 220.f;
        const float MOV_H =  32.f;

        const QVector<QRectF> movRects = {
            {  m_bgWidth * 0.20f,  m_bgHeight * 0.45f,  MOV_W, MOV_H },
            {  m_bgWidth * 0.65f,  m_bgHeight * 0.35f,  MOV_W, MOV_H },
            {  m_bgWidth * 0.10f,  m_bgHeight * 0.25f,  MOV_W, MOV_H },
            {  m_bgWidth * 0.45f,  m_bgHeight * 0.60f,  MOV_W, MOV_H },
            {  m_bgWidth * 1.10f,  m_bgHeight * 0.50f,  MOV_W, MOV_H },
            {  m_bgWidth * 1.50f,  m_bgHeight * 0.40f,  MOV_W, MOV_H }
        };
        /*  ─── Sprites para las plataformas móviles ───  */
        QPixmap tex(":/resources/plataforma.png");

        for (const QRectF& r : movRects)         // ←  usar movRects
        {
            // 1) sprite visible
            QPixmap px = tex.scaled(int(r.width()), int(r.height()),
                                    Qt::IgnoreAspectRatio,
                                    Qt::SmoothTransformation);
            auto* sprite = m_scene->addPixmap(px);
            sprite->setZValue(1);
            sprite->setPos(r.topLeft());

            // 2) hit-box que viajará con el sprite
            auto* hit = m_colManager->addRect(r, Qt::NoBrush, /*collisionOnly=*/true);

            // 3) registrar plataforma móvil
            InvMovingPlatform mp;
            mp.sprite = sprite;
            mp.hitbox = hit;
            mp.minX   = r.x() - 100.f;
            mp.maxX   = r.x() + 100.f;
            mp.speed  = 80.f;
            mp.dir    = +1;
            m_movingPlatforms.append(mp);
        }
    }

    /* ----------------  Enemigos extra  ---------------- */

    // ── Dos gusanos mutantes (zonas media e intermedia‐derecha) ──
    {
        const QVector<QPointF> wormSpawns = {
            /*     X                         Y      */
            {  m_bgWidth * 0.25f,        180.f },   // 1 — zona izquierda alta
            {  m_bgWidth * 0.45f,        230.f },   // 2 — pasarela intermedia
            {  m_bgWidth * 0.70f,        150.f },   // 3 — hueco central
            {  m_bgWidth * 0.95f,        210.f },   // 4 — antes del mid-boss
            {  m_bgWidth * 1.20f,        180.f },   // 5 — mitad del segundo fondo
            {  m_bgWidth * 1.50f,        220.f }    // 6 — cerca del minotauro
        };

        for (const QPointF& p : wormSpawns)
        {
            auto* w = new MutantWorm(this);
            w->setPos(p);           // coloca al gusano
            w->setTarget(m_player); // persigue jugador
            m_scene->addItem(w);
            m_enemigos.append(w);
        }
    }

    // ── Minotauro al final del mapa (jefe) ──
    {
        auto* mino = new Minotaur(this);
        /* ligeramente antes del borde derecho para que quepa */
        mino->setPos(m_bgWidth*2 - 250.f, 150.f);
        mino->setTarget(m_player);
        m_scene->addItem(mino);
        m_enemigos.append(mino);

        /* si quieres que este sea “el” boss del nivel, haz: */
        // m_boss = mino;            // (añade m_boss en tu .h si lo necesitas)
    }



    /* ----------------  Spawn del jugador  ---------------- */
    constexpr float TOP_PLATFORM_Y = 250.f;
    const float spawnX = WALL + GAP + STEP;
    const float spawnY = TOP_PLATFORM_Y - PLAT_H/2;

    if (m_player) {
        m_spawnPos = {spawnX, spawnY};
        m_player->transform().setPosition(spawnX, spawnY);
        m_player->setOnGround(true);

        m_playerItem = new QGraphicsPixmapItem;
        m_playerItem->setZValue(3);
        m_scene->addItem(m_playerItem);
        m_playerItem->setPos(m_spawnPos);

        if (auto* jug = dynamic_cast<Jugador*>(m_player))
            jug->setGraphicsItem(m_playerItem);
    }

    /* ----------------  Boss BringerOfDeath (igual que antes)  -------- */
    auto* boss = new BringerOfDeath(this);
    boss->setPos(4072.33, 651);
    boss->setTarget(m_player);
    m_scene->addItem(boss);
    m_enemigos.append(boss);

    m_debugBossHitbox = new QGraphicsRectItem;
    m_debugBossHitbox->setPen(QPen(Qt::red, 2, Qt::DashLine));
    m_debugBossHitbox->setBrush(Qt::NoBrush);
    m_debugBossHitbox->setZValue(10);

    float barW = 100, barH = 8;
    m_bossHpBorder = new QGraphicsRectItem(0, 0, barW, barH);
    m_bossHpBorder->setPen(QPen(Qt::black));
    m_bossHpBorder->setBrush(Qt::NoBrush);
    m_bossHpBorder->setZValue(11);
    m_bossHpBar = new QGraphicsRectItem(1, 1, barW-2, barH-2);
    m_bossHpBar->setPen(Qt::NoPen);
    m_bossHpBar->setBrush(QColor(200, 0, 0));
    m_bossHpBar->setZValue(12);
    m_scene->addItem(m_bossHpBorder);
    m_scene->addItem(m_bossHpBar);

    /* ----------------  Combate + HUD (sin cambios)  ---------------- */
    if (auto* jug = dynamic_cast<Jugador*>(m_player))
        m_combate = new CombateManager(jug, m_enemigos, this);

    m_hudBorder = new QGraphicsRectItem(0, 0, HUD_W, HUD_H);
    m_hudBorder->setPen(QPen(Qt::black));
    m_hudBorder->setBrush(Qt::NoBrush);
    m_hudBorder->setZValue(100);
    m_hudBorder->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_scene->addItem(m_hudBorder);

    m_hudBar = new QGraphicsRectItem(1, 1, HUD_W-2, HUD_H-2);
    m_hudBar->setPen(Qt::NoPen);
    m_hudBar->setBrush(QColor(50, 205, 50));
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

    /* ----------------  Timer  ---------------- */
    connect(m_timer, &QTimer::timeout, this, &ciudadinversa::onFrame);
    m_timer->start(int(m_dt * 1000));
}

/*  ──────────────────────
    El resto del archivo
    (keyPressEvent, keyReleaseEvent, onFrame, …)
    permanece SIN CAMBIOS.
   ────────────────────── */

/* =========================================================//---------------------------------------------------------------------------------------
 *  Entrada (no cambia)
 * ========================================================= */
void ciudadinversa::keyPressEvent(QKeyEvent* e)
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

/* =========================================================
 *  Loop principal
 * ========================================================= */
void ciudadinversa::onFrame()
{
    if (!m_player) return;

    QPointF footPos1 = m_player->transform().getPosition();
    float x = footPos1.x();
    float y = footPos1.y();

    // 1) Si se sale por la izquierda, volvemos al tutorial
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


