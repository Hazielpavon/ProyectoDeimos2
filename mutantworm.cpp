#include "MutantWorm.h"
#include "jugador.h"

#include <QTransform>
#include <QGraphicsScene>
#include <QtMath>
#include <QDebug>

/* --------------- constantes --------------- */
namespace {
const qreal  SCALE          = 0.5;
const float  GRAVITY        = 600.f;

const float  ATK_RANGE      = 55.f;
const float  DET_RANGE      = 260.f;
const float  Y_TOLERANCE    = 14.f;

const float  JUMP_VY        = -350.f;
const float  JUMP_CD        = 3.f;      // segundos
}

/* helper: carga 000-017 */
static Animacion loadSet(const QString& patt,int frames,bool mirror=false)
{
    Animacion a; a.fps = 9.f;
    QTransform flip; flip.scale(-1,1);

    for (int i=0;i<frames;++i){
        QString file = patt.arg(i,3,10,QChar('0'));
        QPixmap p(file);
        if (p.isNull()) { qWarning()<<"[MutantWorm] Falta"<<file; continue; }
        p = p.scaled(p.size()*SCALE, Qt::KeepAspectRatio,
                     Qt::SmoothTransformation);
        if (mirror) p = p.transformed(flip);
        a.frames.append(p);
    }
    return a;
}

/* --------------- ctor --------------- */
MutantWorm::MutantWorm(QObject* parent):Enemigo(parent)
{
    addAnim(Estado::Idle  , loadSet(":/resources/1_Monster_Idle_%1.png"   ,18));
    addAnim(Estado::Walk  , loadSet(":/resources/1_Monster_Walking_%1.png",18));
    addAnim(Estado::Attack, loadSet(":/resources/1_Monster_Attack_%1.png" ,18));
    addAnim(Estado::Jump  , loadSet(":/resources/1_Monster_Jump_%1.png"  ,18));
    addAnim(Estado::Death , loadSet(":/resources/1_Monster_Dying_%1.png" ,18));

    if (!animActual().frames.isEmpty()) {
        QPixmap f = animActual().frames.first();
        setOffset(-f.width()/2, -f.height()/2);
        setTransformOriginPoint(f.width()/2, f.height()/2);
    }
}

/* --------------- geom --------------- */
QRectF MutantWorm::boundingRect() const
{
    const auto& p = pixmap();
    return {-p.width()/2, -p.height()/2, p.width(), p.height()};
}
QPainterPath MutantWorm::shape() const
{
    QPainterPath s;  s.addRect(boundingRect());  return s;
}

/* --------------- IA --------------- */
void MutantWorm::updateAI(float dt)
{
    m_jumpCooldown -= dt;

    /* salto periódico solo si patrulla y está en suelo */
    if (m_mode == Mode::Patrol && isOnGround() && m_jumpCooldown<=0.f) {
        startJump();
        return;
    }

    if (!target()) return;

    float dx = target()->transform().getPosition().x() - pos().x();

    float selfFeet = sceneBoundingRect().bottom();
    float targFeet = selfFeet;
    if (auto* j = dynamic_cast<Jugador*>(target()))
        if (auto* gi = j->graphicsItem())
            targFeet = gi->sceneBoundingRect().bottom();

    /* ataque */
    if (qAbs(dx)<ATK_RANGE && qAbs(targFeet-selfFeet)<Y_TOLERANCE) {
        setEstado(Estado::Attack); m_velX = 0; m_faceRight = dx>=0;
        m_mode = Mode::Attack; return;
    }
    /* persecución */
    if (qAbs(dx)<DET_RANGE) {
        setEstado(Estado::Walk);
        m_velX = (dx>0?1:-1)*140.f;
        m_faceRight = dx>=0; m_mode=Mode::Chase; return;
    }
    /* patrulla */
    setEstado(Estado::Walk);
    m_patrolTime += dt;
    if (m_patrolTime>2.5f){ m_patrolDir=-m_patrolDir; m_patrolTime=0; }
    m_velX = m_patrolDir*90.f;
    m_faceRight = m_patrolDir>0;  m_mode=Mode::Patrol;
}

/* --------------- salto --------------- */
void MutantWorm::startJump()
{
    setEstado(Estado::Jump);
    m_velY = JUMP_VY;
    m_jumping = true;
    m_jumpCooldown = JUMP_CD;
    m_mode = Mode::Jump;
}

/* --------------- daño / muerte --------------- */
void MutantWorm::takeDamage(int dmg)
{
    if (isDead()) return;
    Enemigo::takeDamage(dmg);
    if (isDead()) startDeath();
}
void MutantWorm::startDeath()
{
    m_deadAnim = true; m_deadTimer=0.f;
    setEstado(Estado::Death);
    auto& a = animActual(); a.idx=0; a.acum=0;
}

/* --------------- update --------------- */
void MutantWorm::update(float dt)
{
    /* IA + física */
    if (!m_deadAnim) {
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
        frame = frame.transformed(QTransform().scale(-1,1));
    setPixmap(frame);
    setOffset(-frame.width()/2, -frame.height()/2);

    /* fin de muerte */
    if (m_deadAnim) {
        m_deadTimer += dt;
        if (m_deadTimer > 1.f/a.fps && a.idx==a.frames.size()-1)
            setVisible(false);
    }
}
