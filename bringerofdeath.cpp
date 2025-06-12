#include "BringerOfDeath.h"
#include "entidad.h"
#include <QDebug>
#include <QString>
#include "jugador.h"
/* ---------- Carga de sprites (Idle/Walk/Attack/Hurt/Death) ---------- */
namespace {
const qreal SCALE = 1.6;
static constexpr float ATTACK_RANGE = 40.0f;
static constexpr float DETECT_RANGE = 250.0f;
static constexpr float Y_TOLERANCE  = 10.0f;
Animacion cargar(const QString& patt, int n, bool mirror = false) {
    Animacion a;
    a.fps = 8.0f;
    QTransform flip;
    if (mirror) {
        // escalar X por -1 para reflejar horizontalmente:
        flip.scale(-1, 1);
    }

    for (int i = 1; i <= n; ++i) {
        QPixmap p(patt.arg(i));
        if (p.isNull()) continue;

        // 1) escalamos al tamaño deseado
        QPixmap scaled = p.scaled(p.size() * SCALE,
                                  Qt::KeepAspectRatio,
                                  Qt::SmoothTransformation);

        // 2) si mirror, aplicamos la transformación
        if (mirror) {
            scaled = scaled.transformed(flip);
        }

        a.frames.append(scaled);
    }
    return a;
}
}
#include <QGraphicsScene>  // para que scene() esté completo

void BringerOfDeath::onDeathAnimFinished()
{
    if (scene()) {
        scene()->removeItem(this);
    }
    deleteLater();
}


QRectF BringerOfDeath::boundingRect() const {
    // Acá devolvemos un rect centrado en (0,0) con el tamaño actual del pixmap:
    const auto& px = pixmap();
    float w = px.width();
    float h = px.height();
    return QRectF(-w/2, -h/2, w, h);
}

QPainterPath BringerOfDeath::shape() const {
    QPainterPath p;
    p.addRect(boundingRect());
    return p;
}
BringerOfDeath::BringerOfDeath(QObject* parent)
    : Enemigo(parent)
{
    addAnim(Estado::Idle  , cargar(":/resources/Bringer-of-Death_Idle_%1.png"  ,8));
    addAnim(Estado::Walk  , cargar(":/resources/Bringer-of-Death_Walk_%1.png"  ,4));
    addAnim(Estado::Attack, cargar(":/resources/Bringer-of-Death_Attack_%1.png",4));
    addAnim(Estado::Hurt  , cargar(":/resources/Bringer-of-Death_Hurt_%1.png"  ,4));
    addAnim(Estado::Death , cargar(":/resources/Bringer-of-Death_Death_%1.png" ,4));

    Animacion& idleAnim = animActual();
    if (!idleAnim.frames.isEmpty()) {
        QPixmap first = idleAnim.frames.first();
        setOffset(-first.width()/2, -first.height()/2);
        setTransformOriginPoint(first.width()/2, first.height()/2);
    }
    m_flipTransform.scale(-1, 1);
}

/* ---------- IA principal ---------- */
// BringerOfDeath.cpp (debes tener definidos ATTACK_RANGE, DETECT_RANGE, Y_TOLERANCE)

void BringerOfDeath::updateAI(float dt)
{
    if (!target()) return;

    // 1) Posiciones en X/Y
    float bossX   = pos().x();
    float playerX = target()->transform().getPosition().x();
    float dx      = playerX - bossX;

    // pies en Y
    float bossFootY = sceneBoundingRect().bottom();
    Jugador* pj = dynamic_cast<Jugador*>(target());
    float playerFootY = pj && pj->graphicsItem()
                            ? pj->graphicsItem()->sceneBoundingRect().bottom()
                            : bossFootY;  // si no podemos, igualamos

    // 2) ATAQUE si entra en rango X/Y
    if (qAbs(dx) < ATTACK_RANGE &&
        qAbs(playerFootY - bossFootY) < Y_TOLERANCE)
    {
        setEstado(Estado::Attack);
        m_velX = 0;
        m_mode  = Mode::Attack;
        // mira al revés de donde esté el jugador
        m_facingRight = !(dx >= 0.0f);
        return;
    }

    // 3) PERSECUCIÓN si está a X<DETECT
    if (qAbs(dx) < DETECT_RANGE) {
        setEstado(Estado::Walk);
        m_velX = (dx > 0 ? +1 : -1) * 90.0f;
        m_mode  = Mode::Chase;
        // mira en misma dirección que la velocidad
        m_facingRight = (dx >= 0.0f);
        return;
    }

    // 4) PATRULLA
    setEstado(Estado::Walk);
    m_patrolTime += dt;
    if (m_patrolTime > 2.0f) {
        m_patrolDir  = -m_patrolDir;
        m_patrolTime = 0.0f;
    }
    m_velX      = m_patrolDir * 60.0f;
    m_mode      = Mode::Patrol;
    // mira hacia donde camina
    m_facingRight = (m_patrolDir > 0);
}
void BringerOfDeath::takeDamage(int dmg)
{
    if (isDead()) return;
    Enemigo::takeDamage(dmg);
    if (isDead()) {
        m_deathStarted  = true;
        m_deathTimer    = 0.0f;
        setEstado(Estado::Death);
        auto &a = animActual();
        a.idx  = 0;
        a.acum = 0.0f;
    }
}

void BringerOfDeath::update(float dt)
{
    // 1) Física y IA normales
    if (!m_deathStarted) {
        updateAI(dt);
        constexpr float GRAV = 600.0f;
        m_velY += GRAV * dt;
        moveBy(m_velX * dt, m_velY * dt);
    }

    // 2) Animación
    auto &anim = animActual();
    anim.avanzar(dt);
    // forzar que no haga loop en Death
    if (m_deathStarted) {
        int last = int(anim.frames.size()) - 1;
        if (anim.idx > last) anim.idx = last;
    }

    // 3) Pintar frame
    QPixmap frame = anim.actual();
    if (m_facingRight)
        frame = frame.transformed(QTransform().scale(-1,1));
    setPixmap(frame);
    setOffset(-frame.width()/2.0, -frame.height()/2.0);

    // 4) Si estamos en Death, ocultamos al terminar y salimos
    if (m_deathStarted) {
        int lastIdx    = int(anim.frames.size()) - 1;
        float frameDur = 1.0f / anim.fps;
        if (anim.idx == lastIdx) {
            m_deathTimer += dt;
            if (m_deathTimer >= frameDur) {
                // solo lo ocultamos, NO lo borramos
                setVisible(false);
                // dejamos que NivelRaicesOlvidadas lo elimine
            }
        }
        return;
    }
}








