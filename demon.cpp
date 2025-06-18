#include "demon.h"
#include "entidad.h"
#include "jugador.h"
#include <QTransform>
#include <QGraphicsScene>
#include <QtMath>
#include <QDebug>

/* ------------ constants ------------ */
namespace {
const qreal SCALE         = 1.6;     // enlarge sprite
const float ATTACK_RANGE  = 45.0f;
const float DETECT_RANGE  = 260.0f;
const float Y_TOLERANCE   = 10.0f;
const float GRAVITY       = 600.0f;

/* helper to load and optionally mirror */
Animacion loadSet(const QString& pattern, int frames, bool mirror = false)
{
    Animacion a;  a.fps = 8.0f;
    QTransform flip;  flip.scale(-1,1);

    for (int i = 1; i <= frames; ++i) {
        QPixmap p(pattern.arg(i));
        if (p.isNull()) continue;
        p = p.scaled(p.size()*SCALE, Qt::KeepAspectRatio,
                     Qt::SmoothTransformation);
        if (mirror) p = p.transformed(flip);
        a.frames.append(p);
    }
    return a;
}
} // anon-ns

/* ---------- ctor ---------- */
Demon::Demon(QObject* parent)
    : Enemigo(parent)
{
    addAnim(Estado::Idle  , loadSet(":/resources/demon_idle_%1.png"   ,  6));
    addAnim(Estado::Walk  , loadSet(":/resources/demon_walk_%1.png"   , 12));
    addAnim(Estado::Attack, loadSet(":/resources/demon_cleave_%1.png" , 15));
    addAnim(Estado::Hurt  , loadSet(":/resources/demon_take_hit_%1.png", 5));
    addAnim(Estado::Death , loadSet(":/resources/demon_death_%1.png"  , 22));

    /* centre anchor */
    if (!animActual().frames.isEmpty()) {
        QPixmap f = animActual().frames.first();
        setOffset(-f.width()/2, -f.height()/2);
        setTransformOriginPoint(f.width()/2, f.height()/2);
    }
}

/* ---------- geometry for collisions ---------- */
QRectF Demon::boundingRect() const
{
    const auto& p = pixmap();
    return QRectF(-p.width()/2, -p.height()/2, p.width(), p.height());
}
QPainterPath Demon::shape() const
{
    QPainterPath s;  s.addRect(boundingRect());  return s;
}

/* ---------- AI ---------- */
void Demon::updateAI(float dt)
{
    if (!target()) return;

    float bossX   = pos().x();
    float playerX = target()->transform().getPosition().x();
    float dx      = playerX - bossX;

    /* Y feet */
    float bossFootY = sceneBoundingRect().bottom();
    float playerFootY = bossFootY;
    if (auto* j = dynamic_cast<Jugador*>(target())) {
        if (auto* gi = j->graphicsItem())
            playerFootY = gi->sceneBoundingRect().bottom();
    }

    /* ----- Attack ----- */
    if (qAbs(dx) < ATTACK_RANGE &&
        qAbs(playerFootY - bossFootY) < Y_TOLERANCE)
    {
        setEstado(Estado::Attack);
        m_velX = 0.0f;
        m_mode = Mode::Attack;
        m_facingRight = (dx >= 0.0f);
        return;
    }

    /* ----- Chase ----- */
    if (qAbs(dx) < DETECT_RANGE) {
        setEstado(Estado::Walk);
        m_velX = (dx > 0 ? +1 : -1) * 100.0f;
        m_mode = Mode::Chase;
        m_facingRight = (dx >= 0.0f);
        return;
    }

    /* ----- Patrol ----- */
    setEstado(Estado::Walk);
    m_patrolTime += dt;
    if (m_patrolTime > 2.5f) {
        m_patrolDir  = -m_patrolDir;
        m_patrolTime = 0.0f;
    }
    m_velX = m_patrolDir * 70.0f;
    m_mode = Mode::Patrol;
    m_facingRight = (m_patrolDir > 0);
}

/* ---------- Damage ---------- */
void Demon::takeDamage(int dmg)
{
    if (isDead()) return;
    Enemigo::takeDamage(dmg);

    if (isDead())
        startDeath();
}

void Demon::startDeath()
{
    m_deathStarted = true;
    m_deathTimer   = 0.0f;
    setEstado(Estado::Death);
    auto& a = animActual();
    a.idx = 0;  a.acum = 0.0f;
}

/* ---------- update ---------- */
void Demon::update(float dt)
{
    /* 1) IA + Física si no muerto */
    if (!m_deathStarted) {
        updateAI(dt);
        m_velY += GRAVITY * dt;
        moveBy(m_velX * dt, m_velY * dt);
    }

    /* 2) Animación */
    auto& an = animActual();
    an.avanzar(dt);
    if (m_deathStarted && an.idx == an.frames.size()-1) {
        /* freeze on last frame */
        an.acum = 0.0f;
    }

    /* 3) Dibujar frame (flip) */
    QPixmap frame = an.actual();
    if (m_facingRight)
        frame = frame.transformed(QTransform().scale(-1,1));
    setPixmap(frame);
    setOffset(-frame.width()/2, -frame.height()/2);

    /* 4) Fin de muerte: ocultar tras animación completa  */
    if (m_deathStarted) {
        m_deathTimer += dt;
        float frameDur = 1.0f / an.fps;
        if (m_deathTimer >= frameDur &&
            an.idx == an.frames.size()-1)
        {
            setVisible(false);  // scene puede borrarlo después
        }
    }

}
