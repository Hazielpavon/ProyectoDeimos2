
#pragma once

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QString>

class entidad;

class Drop : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    enum class Tipo { Vida, Mana, Llave };

    Drop(Tipo tipo,
         const QPointF& posicion,
         QGraphicsScene* scene,
         const QString& nombreLlave = "",
         QObject* parent = nullptr);
    ~Drop();

    bool  checkCollision(entidad* player);
    void  aplicarEfecto(entidad* player);
    bool  isCollected() const;

    Tipo      tipo()         const;
    QString   nombreLlave()  const;

signals:
    void llaveRecogida(const QString& id, const QPixmap& icon);

private:
    Tipo                m_tipo;
    QString             m_nombreLlave;
    QGraphicsPixmapItem* m_sprite = nullptr;
    bool                m_collected = false;
};
