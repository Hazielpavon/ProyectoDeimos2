#pragma once
#include "Enemigo.h"

class Demon : public Enemigo
{
    Q_OBJECT
public:
    explicit Demon(QObject* parent = nullptr);

    /* Enemigo overrides */
    void update(float dt) override;
    void takeDamage(int dmg) override;

    /* Bounding rect so QGraphicsScene can collide */
    QRectF        boundingRect() const override;
    QPainterPath  shape()        const override;
protected:
    void onRevive() override {
        // llamamos al padre ya hecho, luego reseteamos lo propio
        Enemigo::onRevive();
        m_deathStarted = false;      // vuelve a procesar update()
        m_patrolTime = 0;    // opcional, reinicio de patrulla
        m_mode = Mode::Patrol;
    }
private:
    enum class Mode { None, Chase, Patrol, Attack };

    void updateAI(float dt);
    void startDeath();

    /* IA helpers */
    Mode   m_mode        = Mode::None;
    float  m_patrolTime  = 0.0f;
    int    m_patrolDir   = +1;
    bool   m_facingRight = true;
    bool  m_dead;
    /* Death handling */
    bool   m_deathStarted  = false;
    float  m_deathTimer    = 0.0f;
};
