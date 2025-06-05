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
    entidad();
    ~entidad();
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
private:
    void actualizarAnimacion(float dt);
    void actualizarSalto(float dt);
    SpriteState m_animacionTemporal = SpriteState::Idle;
    float m_tiempoAnimacionTemporal = 0.0f;
    tranformacion   m_transformacion;
    Sprite           m_sprite;
    componentefisico m_componenteFisico;
    componentesalud  m_componenteSalud;
    bool m_onGround = false;
    SpriteState m_lastDirection = SpriteState::Idle;
    bool   m_isJumping;
    float  m_verticalVelocity;
    float  m_groundY;
    static constexpr float GRAVITY = 800.0f;
    static constexpr float JUMP_VELOCITY = 450.0f;
    bool   m_facingLeft;

};
