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

    // Llamado desde MainWindow cada frame:
    void actualizar(float dt);

    // Para que MainWindow inicie el salto:
    void startJump();

    // Accesores básicos:
    tranformacion& transform()       { return m_transformacion; }
    componentefisico& fisica()        { return m_componenteFisico; }
    componentesalud& salud()          { return m_componenteSalud; }
    Sprite& sprite()                  { return m_sprite; }

    // Saber si actualmente “mira” a la izquierda:
    bool facingleft() const           { return m_facingLeft; }

private:
    void actualizarAnimacion(float dt);
    void actualizarSalto(float dt);

    tranformacion   m_transformacion;
    Sprite           m_sprite;
    componentefisico m_componenteFisico;
    componentesalud  m_componenteSalud;

    // ←◆◆◆ Atributos nuevos para salto ◆◆◆→
    bool   m_isJumping;         // ¿está en el aire?
    float  m_verticalVelocity;  // velocidad actual en Y (px/s)
    float  m_groundY;           // coordenada Y considerada “suelo” (punto de partida)
    static constexpr float GRAVITY = 800.0f;   // gravedad en px/s²
    static constexpr float JUMP_VELOCITY = 450.0f; // velocidad inicial del salto (px/s hacia arriba)
    bool   m_facingLeft;        // para saber hacia dónde mira al acabar el salto

    // ─────────────────────────────────────────
};
