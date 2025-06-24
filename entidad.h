// entity.h
#pragma once
#include <QPointF>
#include "tranformacion.h"
#include "sprite.h"
#include "componentefisico.h"
#include "componentesalud.h"

class entidad
{
public:
    entidad();
    virtual ~entidad() = default;

    /* -------- actualización y movimiento -------- */
    void actualizar(float dt);
    void startJump();                               // usa m_jumpMultiplier

    /* -------- getters / setters -------- */
    tranformacion&    transform()       { return m_transformacion; }
    componentefisico& fisica()          { return m_componenteFisico; }
    componentesalud&  salud()           { return m_componenteSalud; }
    Sprite&           sprite()          { return m_sprite; }

    void  setOnGround(bool gnd);
    bool  isOnGround()            const;
    void  setLastDirection(SpriteState dir);
    SpriteState getLastDirection() const;
    bool  facingleft()            const { return m_facingLeft; }

    /* --- HP / MP helpers --- */
    int  currentHP() const { return m_componenteSalud.currentHP(); }
    int  maxHP()     const { return m_componenteSalud.maxHP(); }
    void setHP(int v)      { m_componenteSalud.setHP(v); }

    int  Getmana()   { return mana; }
    void Setmana(int m){ mana = m; }
    int  maxMana() const { return 100; }

    /* -------- animación temporal (ataque, etc.) -------- */
    void reproducirAnimacionTemporal(SpriteState st, float seg);

    /* -------- super-salto (skill tree) ----------------- */
    void  setJumpMultiplier(float m) { m_jumpMultiplier = m; }
    float jumpMultiplier() const     { return m_jumpMultiplier; }

private:
    /* helpers internos */
    void actualizarAnimacion(float dt);

    /* -------- datos  ----------------------------------- */
    tranformacion   m_transformacion;
    Sprite          m_sprite;
    componentefisico m_componenteFisico;
    componentesalud  m_componenteSalud;

    /* estado salto / animación */
    bool         m_onGround       = false;
    bool         m_facingLeft     = false;
    SpriteState  m_lastDirection  = SpriteState::Idle;
    SpriteState  m_animTemporal   = SpriteState::Idle;
    float        m_animTempTime   = 0.f;

    /* stats extra */
    float        m_jumpMultiplier = 1.f;   // ← skill “Super Salto”
    int          mana             = 100;

    /* constantes físicas */
    static constexpr float GRAVITY       = 800.f;
    static constexpr float JUMP_VELOCITY = 450.f;
};
