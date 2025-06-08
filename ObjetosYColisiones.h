#pragma once
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <vector>

class entidad;               // ← adelanta la clase de tu jugador

class ObjetosYColisiones : public QObject
{
    Q_OBJECT
public:
    explicit ObjetosYColisiones(QGraphicsScene* scene,
                                QObject* parent = nullptr);

    /* Crea un rectángulo de colisión (HitBox) y,
       opcionalmente, su parte visual.
       area  : posición y tamaño en coordenadas de escena
       color : sólo se usa si collisionOnly == false
       collisionOnly : true ⇒ sólo HitBox, sin dibujo       */
    QGraphicsRectItem* addRect(const QRectF& area,
                               const QColor& color = QColor(80,80,80),
                               bool collisionOnly = false);

    /* Llama una vez por cuadro, después de mover al jugador.
       dt → delta time (segundos) para resolver penetraciones. */
    void resolveCollisions(entidad* player, float dt);

private:
    struct Objeto {
        QGraphicsRectItem* visual;     // nullptr si collisionOnly
        QGraphicsRectItem* hitbox;     // siempre existe
    };

    std::vector<Objeto> m_objetos;
    QGraphicsScene* m_scene;
};
