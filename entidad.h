// entidad.h
#pragma once
#include <QPointF>
#include "tranformacion.h"
#include "sprite.h"
#include "componentefisico.h"
#include "componentesalud.h"

class entidad
{
public:
    virtual ~entidad() = default;
    entidad();
    void actualizar(float dt);
    void startJump();
    void setGroundY(float newGroundY) { m_groundY = newGroundY; }
    tranformacion& transform()       { return m_transformacion; }
    componentefisico& fisica()        { return m_componenteFisico; }
    componentesalud& salud()          { return m_componenteSalud; }
    Sprite& sprite()                  { return m_sprite; }
    void setLastDirection(SpriteState dir);
    SpriteState getLastDirection() const;
    bool facingleft() const           { return m_facingLeft; }
    void setOnGround(bool enSuelo);
    bool isOnGround() const;
    void reproducirAnimacionTemporal(SpriteState estado, float duracionSegundos);
    componentesalud  m_componenteSalud;
    int currentHP() const { return m_componenteSalud.currentHP(); }
    int maxHP()     const { return m_componenteSalud.maxHP(); }
    void setHP(int hp) { m_componenteSalud.setHP(hp); }
    int Getmana(){ return mana;}
    void Setmana(int mana){ this->mana = mana; }
    int maxMana() const { return 100; }
private:
    void actualizarAnimacion(float dt);
    void actualizarSalto(float dt);
    int mana;
    SpriteState m_animacionTemporal = SpriteState::Idle;
    float m_tiempoAnimacionTemporal = 0.0f;
    tranformacion   m_transformacion;
    Sprite           m_sprite;
    componentefisico m_componenteFisico;
    bool m_onGround = false;
    SpriteState m_lastDirection = SpriteState::Idle;
    bool   m_isJumping;
    float  m_verticalVelocity;
    float  m_groundY;
    static constexpr float GRAVITY = -800.0f;
    static constexpr float JUMP_VELOCITY = 450.0f;
    bool   m_facingLeft;

};
