#pragma once
#include <QObject>
#include <QGraphicsPixmapItem>
#include <QVector>
#include <QHash>
#include <QPixmap>
#include <QDebug>

class entidad;   // jugador u otra entidad

/* ---------- Motor de animaciones ---------- */
struct Animacion
{
    QVector<QPixmap> frames;
    float fps  = 8.0f;
    float acum = 0.0f;
    int   idx  = 0;

    bool avanzar(float dt)
    {
        if (frames.isEmpty()) return false;
        acum += dt;
        const float T = 1.0f / fps;
        if (acum >= T) { acum -= T; idx = (idx + 1) % frames.size(); return true; }
        return false;
    }
    const QPixmap& actual() const { return frames[idx]; }
};

/* ---------- Clase base Enemigo ------------------------- */
class Enemigo : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    enum class Estado { Idle, Walk, Attack, Hurt, Death,IdleLeft, WalkLeft, AttackLeft, HurtLeft};

    explicit Enemigo(QObject* parent = nullptr);

    /* IA + animación + física (cada enemigo implementa) */
    virtual void update(float dt) = 0;

    /* -------- Salud -------- */
    virtual void takeDamage(int dmg);
    bool isDead()    const { return m_estado == Estado::Death; }
    int  currentHP() const { return m_hp; }
    int  maxHP()     const { return m_maxHP; }

    /* -------- Estado / frame -------- */
    Estado estado() const { return m_estado; }
    int    frameIndex() const { return animActual().idx; }   // 👈 NUEVO

    /* -------- Velocidades -------- */
    float velX() const { return m_velX; }
    float velY() const { return m_velY; }
    void  setVelY(float v) { m_velY = v; }

    /* -------- Target (jugador) -------- */
    void     setTarget(entidad* t) { m_target = t; }
    entidad* target() const        { return m_target; }
    void setHp(int hp){ m_hp = hp; }
protected:
    /* Animaciones helper */
    void addAnim(Estado st, const Animacion& anim);
    Animacion&       animActual();
    const Animacion& animActual() const;
    void setEstado(Estado st);

    /* ---- Datos internos ---- */
    Estado                   m_estado = Estado::Idle;
    QHash<Estado, Animacion> m_anims;

    float  m_velX = 0.0f;
    float  m_velY = 0.0f;

    int    m_maxHP = 100;
    int    m_hp    = 100;

    entidad* m_target = nullptr;
};
