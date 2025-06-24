
//minotaur.h

#pragma once
#include "Enemigo.h"


class Minotaur : public Enemigo
{
    Q_OBJECT
public:
    explicit Minotaur(QObject* parent = nullptr);

    void update(float dt) override;
    void takeDamage(int dmg) override;


    QRectF boundingRect() const override;
    QRectF extracted(const QPixmap &p) const;
    QPainterPath shape() const override;

protected:
    void onRevive() override {
        Enemigo::onRevive();
        m_dead = false;
        m_patrolTime = 0;
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
