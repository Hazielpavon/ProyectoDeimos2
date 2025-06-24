#pragma once
#include "enemigo.h"

/* ══════════════════════════════════════════════════════
 *  Mutant Worm – simple ground enemy
 *    • Estados: Idle · Walk · Attack · Jump · Dying
 *    • Ahora puede quedar “estacionario” si así lo desea el nivel
 *      (p. ej. en Ciudad Inversa).
 *      – Por defecto, sigue patrullando / atacando como siempre.
 * ══════════════════════════════════════════════════════ */
class MutantWorm : public Enemigo
{
    Q_OBJECT
public:
    explicit MutantWorm(QObject* parent = nullptr);

    /* ------------- API extra (nuevo) ------------- */
    /// Forza al gusano a quedar totalmente quieto (sin patrullar ni atacar).
    /// Úsalo solo en los niveles que lo necesiten; en los demás, **NO** lo llames.
    void setStationary(bool st) { m_stationary = st; }

    /* ------------- Enemigo overrides ------------- */
    void        update(float dt) override;
    void        takeDamage(int dmg) override;
    QRectF      boundingRect() const override;
    QPainterPath shape()        const override;

protected:
    /* se llama cuando el enemigo revive después de morir */
    void onRevive() override
    {
        Enemigo::onRevive();
        m_deadAnim   = false;
        m_patrolTime = 0.f;
        m_mode       = Mode::Patrol;
    }

private:
    /* ---------- IA interna ---------- */
    enum class Mode { None, Patrol, Chase, Attack, Jump };

    void  updateAI(float dt);
    void  startJump();
    void  startDeath();

    /* estado de IA */
    Mode  m_mode        = Mode::None;
    float m_patrolTime  = 0.f;
    int   m_patrolDir   = +1;
    bool  m_faceRight   = true;

    /* salto */
    bool  m_jumping      = false;
    float m_jumpCooldown = 0.f;

    /* muerte */
    bool  m_deadAnim     = false;
    float m_deadTimer    = 0.f;

    /* ---------- NUEVO ---------- */
    bool  m_stationary   = false;   ///< si es true, update() sale inmediatamente
};
