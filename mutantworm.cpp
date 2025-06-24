
#include "mutantWorm.h"
#include "jugador.h"

#include <QTransform>
#include <QGraphicsScene>
#include <QtMath>
#include <QDebug>

/* ---------- constantes internas ---------- */
namespace {
constexpr qreal  SCALE          = 0.5;
constexpr float  GRAVITY        = 600.f;

constexpr float  ATK_RANGE      = 55.f;
constexpr float  DET_RANGE      = 260.f;
constexpr float  Y_TOLERANCE    = 14.f;

constexpr float  JUMP_VY        = -350.f;
constexpr float  JUMP_CD        = 3.f;
}

static Animacion loadSet(const QString& patt, int frames, bool mirror = false)
{
    Animacion a;  a.fps = 9.f;
    QTransform flip; flip.scale(-1, 1);

    for (int i = 0; i < frames; ++i) {
        QString file = patt.arg(i, 3, 10, QChar('0'));
        QPixmap p(file);
        if (p.isNull()) { qWarning() << "[MutantWorm] Falta" << file; continue; }
        p = p.scaled(p.size() * SCALE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        if (mirror) p = p.transformed(flip);
        a.frames.append(p);
    }
    return a;
}

/* ---------- ctor ---------- */
MutantWorm::MutantWorm(QObject* parent)
    : Enemigo(parent)
{
    addAnim(Estado::Idle  , loadSet(":/resources/1_Monster_Idle_%1.png"   , 18));
    addAnim(Estado::Walk  , loadSet(":/resources/1_Monster_Walking_%1.png", 18));
    addAnim(Estado::Attack, loadSet(":/resources/1_Monster_Attack_%1.png" , 18));
    addAnim(Estado::Jump  , loadSet(":/resources/1_Monster_Jump_%1.png"   , 18));
    addAnim(Estado::Death , loadSet(":/resources/1_Monster_Dying_%1.png"  , 18));

    setDamage(8);

    if (!animActual().frames.isEmpty()) {
        const QPixmap& f = animActual().frames.first();
        setOffset(-f.width() / 2, -f.height() / 2);
        setTransformOriginPoint(f.width() / 2, f.height() / 2);
    }
}

/* ---------- bounding / shape ---------- */
QRectF MutantWorm::boundingRect() const
{
    const QPixmap& p = pixmap();
    qreal w = static_cast<qreal>(p.width());
    qreal h = static_cast<qreal>(p.height());
    return QRectF(-w / 2.0, -h / 2.0, w, h);
}
QPainterPath MutantWorm::shape() const
{
    QPainterPath s;  s.addRect(boundingRect());  return s;
}

/* ---------- IA principal ---------- */
void MutantWorm::updateAI(float dt)
{
    /* ===== MODO QUIETO (estacionario) ===== */
    if (m_stationary) {
        if (target()) {
            const float dx = target()->transform().getPosition().x() - pos().x();
            m_faceRight = dx >= 0;                // mira al jugador
            /* animación Idle/Attack según rango */
            float selfFeet = sceneBoundingRect().bottom();
            float targFeet = selfFeet;
            if (auto* j = dynamic_cast<Jugador*>(target()))
                if (auto* gi = j->graphicsItem())
                    targFeet = gi->sceneBoundingRect().bottom();

            if (qAbs(dx) < ATK_RANGE &&
                qAbs(targFeet - selfFeet) < Y_TOLERANCE)
                setEstado(Estado::Attack);
            else
                setEstado(Estado::Idle);
        } else {
            setEstado(Estado::Idle);
        }
        m_velX = 0.f;
        return;
    }

    /* ===== IA NORMAL (patrulla / persecución) ===== */
    m_jumpCooldown -= dt;

    if (m_mode == Mode::Patrol && isOnGround() && m_jumpCooldown <= 0.f) {
        startJump();
        return;
    }

    if (!target()) return;

    const float dx = target()->transform().getPosition().x() - pos().x();

    /* pies */
    float selfFeet = sceneBoundingRect().bottom();
    float targFeet = selfFeet;
    if (auto* j = dynamic_cast<Jugador*>(target()))
        if (auto* gi = j->graphicsItem())
            targFeet = gi->sceneBoundingRect().bottom();

    /* --- ataque --- */
    if (qAbs(dx) < ATK_RANGE && qAbs(targFeet - selfFeet) < Y_TOLERANCE) {
        setEstado(Estado::Attack);
        m_velX      = 0.f;
        m_faceRight = dx < 0;          // ← CORREGIDO
        m_mode      = Mode::Attack;
        return;
    }
    /* --- persecución --- */
    if (qAbs(dx) < DET_RANGE) {
        setEstado(Estado::Walk);
        m_velX      = (dx > 0 ? +1 : -1) * 140.f;
        m_faceRight = dx < 0;          // ← CORREGIDO
        m_mode      = Mode::Chase;
        return;
    }
    /* --- patrulla --- */
    setEstado(Estado::Walk);
    m_patrolTime += dt;
    if (m_patrolTime > 2.5f) { m_patrolDir = -m_patrolDir; m_patrolTime = 0.f; }
    m_velX      = m_patrolDir * 90.f;
    m_faceRight = m_patrolDir < 0;     // ← CORREGIDO
    m_mode      = Mode::Patrol;
}

/* ---------- salto / muerte ---------- */
void MutantWorm::startJump()
{
    setEstado(Estado::Jump);
    m_velY         = JUMP_VY;
    m_jumping      = true;
    m_jumpCooldown = JUMP_CD;
    m_mode         = Mode::Jump;
}

void MutantWorm::takeDamage(int dmg)
{
    if (isDead()) return;
    Enemigo::takeDamage(dmg);
    if (isDead()) startDeath();
}
void MutantWorm::startDeath()
{
    m_deadAnim  = true;
    m_deadTimer = 0.f;
    setEstado(Estado::Death);
    auto& a = animActual();  a.idx = 0; a.acum = 0.f;
}

/* ---------- bucle update ---------- */
void MutantWorm::update(float dt)
{
    if (!m_deadAnim) {

        updateAI(dt);

        m_velY += GRAVITY * dt;

        if (m_stationary)
            m_velX = 0.f;

        moveBy(m_velX * dt, m_velY * dt);

        if (m_jumping && isOnGround()) {
            m_jumping = false;
            setEstado(Estado::Idle);
        }
    }

    /* ---------- animación ---------- */
    auto& a = animActual();
    a.avanzar(dt);

    QPixmap frame = a.actual();

    if (!m_faceRight)
        frame = frame.transformed(QTransform().scale(-1, 1));

    setPixmap(frame);
    setOffset(-frame.width() / 2, -frame.height() / 2);

    /* ---------- fin animación de muerte ---------- */
    if (m_deadAnim) {
        m_deadTimer += dt;
        if (m_deadTimer > 1.f / a.fps && a.idx == a.frames.size() - 1)
            setVisible(false);
    }
}
