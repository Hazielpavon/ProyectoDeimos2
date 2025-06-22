#include "Carnivore.h"
#include "jugador.h"

#include <QTransform>
#include <QGraphicsScene>
#include <QtMath>
#include <QDebug>

/* ─────────── Constantes de comportamiento ─────────── */
namespace {
const qreal  SCALE           = 1.35;
const float  GRAVITY         = 600.f;

const float  ATK_RANGE       = 60.f;
const float  DET_RANGE       = 250.f;
const float  Y_TOLERANCE     = 14.f;

const float  JUMP_VY         = -380.f;
const float  JUMP_COOLDOWN   = 3.f;        // s
const int    FRAMES_PER_ANIM = 18;         // 1 … 18
}

/* helper que carga 1-18 con el espacio y paréntesis */
static Animacion loadSeq(const QString& patt,
                         int frames   = FRAMES_PER_ANIM,
                         bool mirror  = false)
{
    Animacion a;  a.fps = 9.f;
    QTransform flip;  flip.scale(-1, 1);

    for (int i = 1; i <= frames; ++i) {
        QString file = patt.arg(i);               // “%1”, sin ceros
        QPixmap p(file);
        if (p.isNull()) {
            qWarning() << "[Carnivore] Falta" << file;
            continue;
        }
        p = p.scaled(p.size() * SCALE,
                     Qt::KeepAspectRatio,
                     Qt::SmoothTransformation);
        if (mirror) p = p.transformed(flip);
        a.frames.append(p);
    }
    return a;
}

/* ─────────── Constructor ─────────── */
Carnivore::Carnivore(QObject* parent) : Enemigo(parent)
{
    addAnim(Estado::Idle  , loadSeq(":/resources/2_Monster_Idle_ (%1).png"));
    addAnim(Estado::Walk  , loadSeq(":/resources/2_Monster_Walking_ (%1).png"));
    addAnim(Estado::Attack, loadSeq(":/resources/2_Monster_Attack_ (%1).png"));
    addAnim(Estado::Jump  , loadSeq(":/resources/2_Monster_Jump_ (%1).png"));
    addAnim(Estado::Death , loadSeq(":/resources/2_Monster_Dying_ (%1).png"));

  setDamage(8);
    if (!animActual().frames.isEmpty()) {
        const QPixmap& f = animActual().frames.first();
        setOffset(-f.width() / 2, -f.height() / 2);
        setTransformOriginPoint(f.width() / 2, f.height() / 2);
    }
}

/* ─────────── Colisión Qt ─────────── */
QRectF Carnivore::boundingRect() const
{
    const auto& p = pixmap();
    return QRectF(
        -p.width()  / 2.0,  // ahora en qreal
        -p.height() / 2.0,
        p.width(),
        p.height()
        );
}

QPainterPath Carnivore::shape() const
{
    QPainterPath s;  s.addRect(boundingRect());  return s;
}

/* ─────────── IA principal ─────────── */
void Carnivore::updateAI(float dt)
{
    m_jumpCD -= dt;

    /* salto ocasional mientras patrulla */
    if (m_mode == Mode::Patrol && isOnGround() && m_jumpCD <= 0.f) {
        startJump();  return;
    }

    if (!target()) return;

    float dx = target()->transform().getPosition().x() - pos().x();

    float selfFeet = sceneBoundingRect().bottom();
    float targFeet = selfFeet;
    if (auto* j = dynamic_cast<Jugador*>(target()))
        if (auto* gi = j->graphicsItem())
            targFeet = gi->sceneBoundingRect().bottom();

    /* ataque */
    if (qAbs(dx) < ATK_RANGE && qAbs(targFeet - selfFeet) < Y_TOLERANCE) {
        setEstado(Estado::Attack);
        m_velX = 0;   m_faceRight = dx >= 0;  m_mode = Mode::Attack;
        return;
    }

    /* persecución */
    if (qAbs(dx) < DET_RANGE) {
        setEstado(Estado::Walk);
        m_velX = (dx > 0 ? 1 : -1) * 150.f;
        m_faceRight = dx >= 0;   m_mode = Mode::Chase;   return;
    }

    /* patrulla */
    setEstado(Estado::Walk);
    m_patrolTime += dt;
    if (m_patrolTime > 2.5f) { m_patrolDir = -m_patrolDir; m_patrolTime = 0; }
    m_velX = m_patrolDir * 100.f;
    m_faceRight = m_patrolDir > 0;  m_mode = Mode::Patrol;
}

/* ─────────── salto ─────────── */
void Carnivore::startJump()
{
    setEstado(Estado::Jump);
    m_velY   = JUMP_VY;
    m_jumping = true;
    m_jumpCD  = JUMP_COOLDOWN;
    m_mode    = Mode::Jump;
}

/* ─────────── daño / muerte ─────────── */
void Carnivore::takeDamage(int dmg)
{
    if (isDead()) return;
    Enemigo::takeDamage(dmg);
    if (isDead()) startDeath();
}
void Carnivore::startDeath()
{
    m_dying    = true;
    m_dieTimer = 0.f;
    setEstado(Estado::Death);
    auto& a = animActual();  a.idx = 0;  a.acum = 0;
}

/* ─────────── update ─────────── */
void Carnivore::update(float dt)
{
    /* IA + física (si vivo) */
    if (!m_dying) {
        if (!m_jumping) updateAI(dt);
        m_velY += GRAVITY * dt;
        moveBy(m_velX * dt, m_velY * dt);

        /* aterrizaje */
        if (m_jumping && isOnGround()) {
            m_jumping = false;
            setEstado(Estado::Idle);
        }
    }

    /* animación */
    auto& a = animActual();
    a.avanzar(dt);

    QPixmap frame = a.actual();
    if (m_faceRight)
        frame = frame.transformed(QTransform().scale(-1, 1));

    setPixmap(frame);
    setOffset(-frame.width() / 2, -frame.height() / 2);

    /* fin de muerte */
    if (m_dying) {
        m_dieTimer += dt;
        if (m_dieTimer > 1.f / a.fps && a.idx == a.frames.size() - 1)
            setVisible(false);
    }
}
