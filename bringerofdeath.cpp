#include "BringerOfDeath.h"
#include "entidad.h"
#include <QDebug>
#include <QString>

/* ---------- Carga de sprites (Idle/Walk/Attack/Hurt/Death) ---------- */
namespace {
const qreal SCALE = 1.6;
Animacion cargar(const QString& patt,int n){
    Animacion a; a.fps=8.0f;
    for(int i=1;i<=n;++i){
        QPixmap p(patt.arg(i));
        if(!p.isNull())
            a.frames.append(p.scaled(p.size()*SCALE,
                                     Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation));
    }
    return a;
}
}

BringerOfDeath::BringerOfDeath(QObject* parent)
    : Enemigo(parent)
{
    addAnim(Estado::Idle  , cargar(":/resources/Bringer-of-Death_Idle_%1.png"  ,8));
    addAnim(Estado::Walk  , cargar(":/resources/Bringer-of-Death_Walk_%1.png"  ,4));
    addAnim(Estado::Attack, cargar(":/resources/Bringer-of-Death_Attack_%1.png",4));
    addAnim(Estado::Hurt  , cargar(":/resources/Bringer-of-Death_Hurt_%1.png"  ,4));
    addAnim(Estado::Death , cargar(":/resources/Bringer-of-Death_Death_%1.png" ,4));

    setOffset(-pixmap().width()/2, -pixmap().height()/2);
}

/* ---------- IA principal ---------- */
void BringerOfDeath::updateAI(float dt)
{
    if (!target()) return;

    float dx = target()->transform().getPosition().x() - pos().x();
    float dist = std::abs(dx);

    /* Muerto */
    if (isDead())           { m_velX = 0.0f; return; }

    /* Si está recibiendo daño, deja pasar la animación Hurt 0.3 s */
    if (m_estado == Estado::Hurt) {
        m_patrolTime += dt;
        if (m_patrolTime >= 0.3f) { m_patrolTime = 0.0f; setEstado(Estado::Idle); }
        return;
    }

    /* Ataque si está muy cerca */
    constexpr float ATTACK_RANGE = 40.0f;
    if (dist < ATTACK_RANGE) {
        setEstado(Estado::Attack);
        m_velX = 0.0f;
        return;
    }

    /* Persecución si está dentro de 250 px */
    constexpr float DETECT_RANGE = 250.0f;
    if (dist < DETECT_RANGE) {
        m_velX = (dx > 0 ? 1 : -1) * 90.0f;
        setEstado(Estado::Walk);
        return;
    }

    /* Patrulla simple (cambia de dirección cada 2 s) */
    m_patrolTime += dt;
    if (m_patrolTime > 2.0f) { m_patrolDir = -m_patrolDir; m_patrolTime = 0.0f; }
    m_velX = m_patrolDir * 60.0f;
    setEstado(Estado::Walk);
}

/* ---------- Bucle update ---------- */
void BringerOfDeath::update(float dt)
{
    /* IA decide velocidad y estado */
    updateAI(dt);

    /* 1) Animación */
    Animacion& anim = animActual();
    if (anim.avanzar(dt)) setPixmap(anim.actual());

    /* 2) Física básica (solo gravedad + velX de IA) */
    constexpr float GRAV = 600.0f;
    m_velY += GRAV * dt;
    moveBy(m_velX * dt, m_velY * dt);
}
