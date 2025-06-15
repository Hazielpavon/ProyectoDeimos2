#include "Minotaur.h"
#include "jugador.h"

#include <QTransform>
#include <QGraphicsScene>
#include <QtMath>
#include <QDebug>

/* ----------------  constantes  ---------------- */
namespace {
const qreal SCALE         = 1.8;        // un poco más grande que Demon
const float ATK_RANGE     = 75.f;       // rango de su hacha
const float DET_RANGE     = 300.f;
const float Y_TOLERANCE   = 12.f;
const float GRAVITY       = 600.f;

/* helper para cargar secuencias individuales */
Animacion loadSet(const QString& pattern, int frames, bool mirror=false)
{
    Animacion a; a.fps = 8.f;
    QTransform flip; flip.scale(-1,1);

    for (int i=1; i<=frames; ++i) {
        QPixmap p(pattern.arg(i));
        if (p.isNull()) {
            qWarning() << "[Minotaur] Falta:" << pattern.arg(i);
            continue;
        }
        p = p.scaled(p.size()*SCALE, Qt::KeepAspectRatio,
                     Qt::SmoothTransformation);
        if (mirror) p = p.transformed(flip);
        a.frames.append(p);
    }
    return a;
}
} // namespace

/* ----------------  ctor  ---------------- */
Minotaur::Minotaur(QObject* parent) : Enemigo(parent)
{
    addAnim(Estado::Idle  , loadSet(":/resources/idle_%1.png" , 16));
    addAnim(Estado::Walk  , loadSet(":/resources/walk_%1.png" , 12));
    addAnim(Estado::Attack, loadSet(":/resources/atk_1_%1.png", 16));

    /* si quieres anim de Hurt/Death, añade cuando tengas sprites */
    /* centre anchor */
    if (!animActual().frames.isEmpty()) {
        QPixmap f = animActual().frames.first();
        setOffset(-f.width()/2, -f.height()/2);
        setTransformOriginPoint(f.width()/2, f.height()/2);
    }
}

/* ----------------  geometry  ---------------- */
QRectF Minotaur::boundingRect() const
{
    const auto& p = pixmap();
    return {-p.width()/2, -p.height()/2, p.width(), p.height()};
}
QPainterPath Minotaur::shape() const
{
    QPainterPath s;  s.addRect(boundingRect());  return s;
}

/* ----------------  IA  ---------------- */
void Minotaur::updateAI(float dt)
{
    if (!target()) return;

    float dx = target()->transform().getPosition().x() - pos().x();

    float selfFeet  = sceneBoundingRect().bottom();
    float targFeet  = selfFeet;
    if (auto* j = dynamic_cast<Jugador*>(target()))
        if (auto* gi = j->graphicsItem())
            targFeet = gi->sceneBoundingRect().bottom();

    /* ataque */
    if (qAbs(dx) < ATK_RANGE &&
        qAbs(targFeet - selfFeet) < Y_TOLERANCE)
    {
        setEstado(Estado::Attack);
        m_velX = 0;            m_mode = Mode::Attack;
        m_facingRight = dx >= 0; return;
    }

    /* persecución */
    if (qAbs(dx) < DET_RANGE) {
        setEstado(Estado::Walk);
        m_velX = (dx>0 ? 1 : -1) * 120.f;
        m_mode = Mode::Chase;  m_facingRight = dx>=0;  return;
    }

    /* patrulla */
    setEstado(Estado::Walk);
    m_patrolTime += dt;
    if (m_patrolTime > 3.f) { m_patrolDir=-m_patrolDir; m_patrolTime=0; }
    m_velX = m_patrolDir * 80.f;
    m_mode = Mode::Patrol;     m_facingRight = m_patrolDir>0;
}

/* ----------------  daño / muerte (placeholder)  ---------------- */
void Minotaur::takeDamage(int dmg)
{
    Enemigo::takeDamage(dmg);   // sin animaciones de Hurt/Death aún
}

/* ----------------  update ---------------- */
void Minotaur::update(float dt)
{
    /* IA + física */
    updateAI(dt);
    m_velY += GRAVITY*dt;
    moveBy(m_velX*dt, m_velY*dt);

    /* animación */
    auto& an = animActual();
    an.avanzar(dt);

    QPixmap frame = an.actual();
    if (m_facingRight)
        frame = frame.transformed(QTransform().scale(-1,1));

    setPixmap(frame);
    setOffset(-frame.width()/2, -frame.height()/2);
}
