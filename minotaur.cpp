//minotaur.cpp

#include "minotaur.h"
#include "jugador.h"

#include <QTransform>
#include <QGraphicsScene>
#include <QtMath>
#include <QDebug>

namespace {
const qreal SCALE         = 1.5;
const float ATK_RANGE     = 75.f;
const float DET_RANGE     = 300.f;
const float Y_TOLERANCE   = 12.f;
const float GRAVITY       = 600.f;

static Animacion loadSet(const QString& pat,int n,bool mirror=false)
{
    Animacion a; a.fps = 8.f;
    QTransform flip; flip.scale(-1,1);

    for(int i=1;i<=n;++i){
        QPixmap p(pat.arg(i));
        if(p.isNull()){
            qWarning()<<"[Minotaur] Falta"<<pat.arg(i); continue;
        }
        p = p.scaled(p.size()*SCALE, Qt::KeepAspectRatio,
                     Qt::SmoothTransformation);
        if(mirror) p = p.transformed(flip);
        a.frames.append(p);
    }
    return a;
}
} // anon

Minotaur::Minotaur(QObject* parent) : Enemigo(parent), m_dead(false)
    , m_mode(Mode::Patrol)
    , m_patrolDir(+1)
    , m_patrolTime(0.0f)
    , m_facingRight(true)
{
    addAnim(Estado::Idle  , loadSet(":/resources/idle_%1.png" ,16));
    addAnim(Estado::Walk  , loadSet(":/resources/walk_%1.png" ,12));
    addAnim(Estado::Attack, loadSet(":/resources/atk_1_%1.png",16));
 setDamage(8);
    if(!animActual().frames.isEmpty()){
        const QPixmap& f = animActual().frames.first();
        setOffset(-f.width()/2, -f.height()/2);
        setTransformOriginPoint(f.width()/2, f.height()/2);
    }
}

/* ----------------  hitbox  ---------------- */
QRectF Minotaur::boundingRect() const
{
    const auto& p = pixmap();
    return { -p.width()/2.0, -p.height()/2.0, static_cast<qreal>(p.width()), static_cast<qreal>(p.height()) };

}
QPainterPath Minotaur::shape() const
{
    QPainterPath s; s.addRect(boundingRect()); return s;
}

/* ----------------  IA  ---------------- */
void Minotaur::updateAI(float dt)
{
    if (!target()) return;

    float dx = target()->transform().getPosition().x() - pos().x();

    float selfFeet = sceneBoundingRect().bottom();
    float trgFeet  = selfFeet;
    if (auto* j = dynamic_cast<Jugador*>(target()))
        if (auto* gi = j->graphicsItem())
            trgFeet = gi->sceneBoundingRect().bottom();

    /* ataque */
    if (qAbs(dx)<ATK_RANGE && qAbs(trgFeet-selfFeet)<Y_TOLERANCE){
        setEstado(Estado::Attack);
        m_velX = 0;  m_mode = Mode::Attack;
        m_facingRight = dx>=0; return;
    }

    /* persecución */
    if (qAbs(dx)<DET_RANGE){
        setEstado(Estado::Walk);
        m_velX = (dx>0?1:-1)*120.f;
        m_mode = Mode::Chase; m_facingRight = dx>=0; return;
    }

    /* patrulla */
    setEstado(Estado::Walk);
    m_patrolTime += dt;
    if (m_patrolTime>3.f){ m_patrolDir=-m_patrolDir; m_patrolTime=0; }
    m_velX = m_patrolDir*80.f;
    m_mode = Mode::Patrol; m_facingRight = m_patrolDir>0;
}

/* ----------------  daño / muerte  ---------------- */
void Minotaur::takeDamage(int dmg)
{
    if (m_dead) return;               // ya muerto
    Enemigo::takeDamage(dmg);

    if (isDead()) {
        m_dead = true;
        setVisible(false);            // sin animación: se esfuma
        /* opcional: emitir señal defeated() */
    }
}

/* ----------------  update  ---------------- */
void Minotaur::update(float dt)
{
    if (m_dead) return;               // no procesa nada tras morir

    updateAI(dt);
    m_velY += GRAVITY * dt;
    moveBy(m_velX * dt, m_velY * dt);

    /* animación */
    auto& a = animActual();
    a.avanzar(dt);

    QPixmap frame = a.actual();
    if (m_facingRight)
        frame = frame.transformed(QTransform().scale(-1,1));

    setPixmap(frame);
    setOffset(-frame.width()/2, -frame.height()/2);
}
