// entidad.h
#pragma once

#include <QPointF>
#include "sprite.h"
#include "componentefisico.h"
#include "componentesalud.h"

class entidad
{
public:
    entidad();
    ~entidad();

    void startJump();
    void startAttack();

    void actualizar(float dt);
    void actualizarSalto(float dt);
    void actualizarAnimacion(float dt);

    bool Isjumping() const { return m_isJumping; }
    bool IsAttacking() const { return m_isAttacking; }
    bool facingleft() const { return m_facingLeft; }

    Sprite       &sprite()       { return m_sprite; }
    const Sprite &sprite() const { return m_sprite; }

    tranformacion    &transform()    { return m_transformacion; }
    const tranformacion &transform() const { return m_transformacion; }

    componentefisico       &fisica()       { return m_componenteFisico; }
    const componentefisico &fisica() const { return m_componenteFisico; }

    componentesalud        &salud()        { return m_componenteSalud; }
    const componentesalud  &salud() const  { return m_componenteSalud; }

private:
    tranformacion    m_transformacion;
    Sprite       m_sprite;
    componentefisico       m_componenteFisico;
    componentesalud        m_componenteSalud;

    bool         m_isJumping;
    bool         m_isAttacking;
    float        m_verticalVelocity;
    float        m_groundY;
    bool         m_facingLeft;
    // constantes para salto
    static constexpr float JUMP_VELOCITY = 350.0f;
    static constexpr float GRAVITY       = 980.0f;
};
