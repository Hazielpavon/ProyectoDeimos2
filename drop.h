
#pragma once

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QString>

class entidad;

/* -----------------------------------------------------------
 *  Clase Drop
 *  (pequeños objetos que sueltan los enemigos/jefes)
 * ----------------------------------------------------------- */
class Drop : public QObject
{
    Q_OBJECT
public:
    enum class Tipo { Vida, Mana, Llave };

    Drop(Tipo tipo,
         const QPointF& posicion,
         QGraphicsScene* scene,
         const QString& nombreLlave = "",
         QObject* parent = nullptr);
    ~Drop();

    /* lógica */
    bool  checkCollision(entidad* player);
    void  aplicarEfecto(entidad* player);
    bool  isCollected() const;

    /* acceso */
    Tipo      tipo()         const;
    QString   nombreLlave()  const;

signals:
    /* se emite SOLO si el drop es Tipo::Llave */
    void llaveRecogida(const QString& id, const QPixmap& icon);

private:
    Tipo                m_tipo;
    QString             m_nombreLlave;      // clave identificadora
    QGraphicsPixmapItem* m_sprite = nullptr;
    bool                m_collected = false;
};
