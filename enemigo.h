// ===========================================================
//  Enemigo.h – cabecera unificada y retro-compatible
// ===========================================================
#pragma once

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QVector>
#include <QMap>
#include <QPixmap>

/* -----------------------------------------------------------
 *  Animación sencilla
 * ----------------------------------------------------------- */
struct Animacion
{
    QVector<QPixmap> frames;
    qreal fps  = 8.0;   // fotogramas/seg
    int   idx  = 0;     // índice frame actual
    qreal acum = 0.0;   // acumulador de tiempo

    bool avanzar(qreal dt)               // true si cambió frame
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

/* -----------------------------------------------------------
 *  Adelanto
 * ----------------------------------------------------------- */
class entidad;

/* -----------------------------------------------------------
 *  Clase base para todos los enemigos
 * ----------------------------------------------------------- */
class Enemigo : public QObject,
                public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    /* ---------- Estados ---------- */
    enum class Estado {
        Idle,
        Walk,
        Attack,
        Hurt,
        Death,
        Jump          // ← NUEVO
    };

    virtual void revive(int hp);
    explicit Enemigo(QObject* parent = nullptr);
    int damage() const { return m_damage; }
    void setDamage(int d) { m_damage = d; }
    /* ---------- Animación ---------- */
    void          addAnim(Estado st, const Animacion& a);
    void          setEstado(Estado st);
    Estado        estado() const { return m_estado; }

    Animacion&          animActual();             // mutable
    const Animacion&    animActual() const;       // const overload

    /* ---------- Lógica de vida ---------- */
    virtual void update(float dt) = 0;            // ≈ tick 60 FPS
    virtual void takeDamage(int dmg);             // resta vida

    bool isDead()    const { return m_hp <= 0; }
    int  currentHP() const { return m_hp; }
    int  maxHP()     const { return m_maxHP; }

    /* ---------- Física ---------- */
    qreal velX() const { return m_velX; }
    qreal velY() const { return m_velY; }

    void  setVel(qreal vx, qreal vy) { m_velX=vx; m_velY=vy; }
    /* WRAPPERS retro-compatibles */
    void  setVelX(qreal vx) { m_velX = vx; }
    void  setVelY(qreal vy) { m_velY = vy; }

    /* ---------- Suelo ---------- */
    void  setOnGround(bool v) { m_onGround = v; }
    bool  isOnGround()  const { return m_onGround; }

    /* ---------- Objetivo (jugador) ---------- */
    void     setTarget(entidad* t) { m_target = t; }
    entidad* target() const        { return m_target; }

    /* ---------- Compatibilidad antigua ---------- */
    int  frameIndex() const { return animActual().idx; }   // usado por CombateManager
    void setHP(int hp){ this-> m_hp = hp;}
    virtual void onRevive() {}
protected:
    /* Velocidad para subclases */
    qreal m_velX = 0.0;
    qreal m_velY = 0.0;


private:
    /* Animaciones */
    QMap<Estado, Animacion> m_anims;
    Estado  m_estado = Estado::Idle;

    /* Vida */
    int m_damage = 1;
    int     m_maxHP  = 5;
    int     m_hp     = 5;
    bool   m_isDead = false;
    /* Lógica general */
    entidad* m_target    = nullptr;
    bool     m_onGround  = false;
};
