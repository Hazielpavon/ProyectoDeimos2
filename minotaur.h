
//minotaur.h

#pragma once
#include "Enemigo.h"

/* ════════════════════════════════════════════════════════ *
 *  Minotauro — jefe sin animación de muerte                *
 *  (al morir simplemente desaparece)                       *
 * ════════════════════════════════════════════════════════ */
class Minotaur : public Enemigo
{
    Q_OBJECT
public:
    explicit Minotaur(QObject* parent = nullptr);

    /* Enemigo overrides */
    void update(float dt) override;
    void takeDamage(int dmg) override;


    QRectF boundingRect() const override;
    QRectF extracted(const QPixmap &p) const;
    QPainterPath shape() const override;

protected:
    void onRevive() override {
        // llamamos al padre ya hecho, luego reseteamos lo propio
        Enemigo::onRevive();
        m_dead = false;      // vuelve a procesar update()
        m_patrolTime = 0;    // opcional, reinicio de patrulla
        m_mode = Mode::Patrol;
    }
private:
    enum class Mode { None, Chase, Patrol, Attack };
    void updateAI(float dt);
    Mode  m_mode        = Mode::None;
    float m_patrolTime  = 0.f;
    int   m_patrolDir   = +1;
    bool  m_facingRight = true;
    bool  m_dead        = false;
};
