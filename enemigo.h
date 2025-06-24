
#pragma once

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QVector>
#include <QMap>
#include <QPixmap>


struct Animacion
{
    QVector<QPixmap> frames;
    qreal fps  = 8.0;
    int   idx  = 0;
    qreal acum = 0.0;

    bool avanzar(qreal dt)
    {
        if (frames.isEmpty()) return false;
        bool changed = false;
        const qreal dur = 1.0 / fps;
        acum += dt;
        while (acum >= dur) {
            acum -= dur;
            idx  = (idx + 1) % frames.size();
            changed = true;
        }
        return changed;
    }
    const QPixmap& actual() const
    {
        static QPixmap dummy;
        return frames.isEmpty() ? dummy : frames[idx];
    }
};


class entidad;

class Enemigo : public QObject,
                public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    enum class Estado {
        Idle,
        Walk,
        Attack,
        Hurt,
        Death,
        Jump
    };

    virtual void revive(int hp);
    explicit Enemigo(QObject* parent = nullptr);
    int damage() const { return m_damage; }
    void setDamage(int d) { m_damage = d; }
    void          addAnim(Estado st, const Animacion& a);
    void          setEstado(Estado st);
    Estado        estado() const { return m_estado; }
    Animacion&          animActual();
    const Animacion&    animActual() const;
    virtual void update(float dt) = 0;
    virtual void takeDamage(int dmg);
    bool isDead()    const { return m_hp <= 0; }
    int  currentHP() const { return m_hp; }
    int  maxHP()     const { return m_maxHP; }
    qreal velX() const { return m_velX; }
    qreal velY() const { return m_velY; }
    void  setVel(qreal vx, qreal vy) { m_velX=vx; m_velY=vy; }
    void  setVelX(qreal vx) { m_velX = vx; }
    void  setVelY(qreal vy) { m_velY = vy; }
    void  setOnGround(bool v) { m_onGround = v; }
    bool  isOnGround()  const { return m_onGround; }
    void     setTarget(entidad* t) { m_target = t; }
    entidad* target() const        { return m_target; }
    int  frameIndex() const { return animActual().idx; }
    void setHP(int hp){ this-> m_hp = hp;}
    virtual void onRevive() {}
protected:
    qreal m_velX = 0.0;
    qreal m_velY = 0.0;
private:
    QMap<Estado, Animacion> m_anims;
    Estado  m_estado = Estado::Idle;
    int m_damage = 1;
    int     m_maxHP  = 5;
    int     m_hp     = 5;
    bool   m_isDead = false;
    entidad* m_target    = nullptr;
    bool     m_onGround  = false;
};
