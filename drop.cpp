#include "drop.h"
#include "entidad.h"
#include "InventoryWidget.h"
#include <QGraphicsScene>
#include <QDebug>

Drop::Drop(Tipo tipo,
           const QPointF& pos,
           QGraphicsScene* scene,
           const QString& nombreLlave,
           QObject* parent)
    : QObject(parent)
    , m_tipo(tipo)
    , m_nombreLlave(nombreLlave)
{

    QString path;
    switch (tipo) {
    case Tipo::Vida : path = ":/resources/drop_vida.png";   break;
    case Tipo::Mana : path = ":/resources/drop_mana.png";   break;
    case Tipo::Llave: path = ":/resources/drop_llave.png";  break;
    }

    QPixmap pix(path);
    if (pix.isNull()) { qWarning() << "[Drop] no pudo cargar" << path; return; }

    pix = pix.scaled(pix.size()*0.20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_sprite = new QGraphicsPixmapItem(pix);
    m_sprite->setOffset(-pix.width()/2.0, -pix.height()/2.0);
    m_sprite->setPos(pos);
    m_sprite->setZValue(4);
    if (scene) scene->addItem(m_sprite);

    if (m_tipo == Tipo::Llave) {
        connect(this,  &Drop::llaveRecogida,
                InventoryWidget::instance(),
                &InventoryWidget::addKey);
    }
}

Drop::~Drop()
{
    if (m_sprite && m_sprite->scene())
        m_sprite->scene()->removeItem(m_sprite);
    delete m_sprite;
}

bool Drop::checkCollision(entidad* player)
{
    if (!m_sprite || !player) return false;

    QRectF d = m_sprite->sceneBoundingRect();
    QRectF p(player->transform().getPosition().x() - 20,
             player->transform().getPosition().y() - 50,
             40, 50);

    return d.intersects(p);
    QRectF dropRect = m_sprite->sceneBoundingRect();
    QRectF playerRect(player->transform().getPosition().x() - 20, player->transform().getPosition().y() - 50,
                      40, 50);
    return dropRect.intersects(playerRect);

}

void Drop::aplicarEfecto(entidad* player)
{
    if (!player) return;

    switch (m_tipo) {
    case Tipo::Vida : player->setHP   (player->maxHP());   break;
    case Tipo::Mana : player->Setmana (player->maxMana()); break;
    case Tipo::Llave:
        emit llaveRecogida(m_nombreLlave,
                           m_sprite ? m_sprite->pixmap() : QPixmap());
        break;
    }

    m_collected = true;
    if (m_sprite && m_sprite->scene())
        m_sprite->scene()->removeItem(m_sprite);
    delete m_sprite;
    m_sprite = nullptr;
}

bool    Drop::isCollected () const { return m_collected; }
Drop::Tipo Drop::tipo()     const { return m_tipo; }
QString Drop::nombreLlave() const { return m_nombreLlave; }
