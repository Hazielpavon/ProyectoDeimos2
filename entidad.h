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
    void actualizar(float dt);
    void startJump();
    tranformacion&    transform()       { return m_transformacion; }
    componentefisico& fisica()          { return m_componenteFisico; }
    componentesalud&  salud()           { return m_componenteSalud; }
    Sprite&           sprite()          { return m_sprite; }
    void  setOnGround(bool gnd);
    bool  isOnGround()            const;
    void  setLastDirection(SpriteState dir);
    SpriteState getLastDirection() const;
    bool  facingleft()            const { return m_facingLeft; }
    int  currentHP() const { return m_componenteSalud.currentHP(); }
    int  maxHP()     const { return m_componenteSalud.maxHP(); }
    void setHP(int v)      { m_componenteSalud.setHP(v); }
    int  Getmana()   { return mana; }
    void Setmana(int m){ mana = m; }
    int  maxMana() const { return 100; }
    void reproducirAnimacionTemporal(SpriteState st, float seg);
    void  setJumpMultiplier(float m) { m_jumpMultiplier = m; }
    float jumpMultiplier() const     { return m_jumpMultiplier; }

private:
    void actualizarAnimacion(float dt);
    tranformacion   m_transformacion;
    Sprite          m_sprite;
    componentefisico m_componenteFisico;
    componentesalud  m_componenteSalud;
    bool         m_onGround       = false;
    bool         m_facingLeft     = false;
    SpriteState  m_lastDirection  = SpriteState::Idle;
    SpriteState  m_animTemporal   = SpriteState::Idle;
    float        m_animTempTime   = 0.f;
    float        m_jumpMultiplier = 1.f;
    int          mana             = 100;
    static constexpr float GRAVITY       = 800.f;
    static constexpr float JUMP_VELOCITY = 450.f;
};
