#include "mapawidget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>

MapaWidget::MapaWidget(const QString &regionInicial, QWidget *parent)
    : QWidget(parent),
    m_jugadorSprite(new Sprite),
    m_timer(new QTimer(this)),
    m_regionActual(regionInicial)
{
    setFixedSize(950, 650);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    m_imagenMapa.load(":/resources/mapa_centrado.png");
    if (m_imagenMapa.isNull()) {
        qDebug() << "⚠️ No se pudo cargar mapa_centrado.png";
    }

    m_grafo.cargarRegiones();
    m_grafo.conectarRegiones();

    m_jugadorSprite->loadFrames(SpriteState::Idle, ":/resources/0_Blood_Demon_Idle_", 10);
    m_jugadorSprite->setSize(90, 90);
    m_jugadorSprite->setState(SpriteState::Idle);
    m_jugadorSprite->setFPS(6);

    QPoint pos = m_grafo.posicionRegion(m_regionActual);
    m_jugadorSprite->setPosition(pos.x(), pos.y());

    connect(m_timer, &QTimer::timeout, this, &MapaWidget::onFrame);
    m_timer->start(1000 / 60);
}

MapaWidget::~MapaWidget()
{
    delete m_jugadorSprite;
}

void MapaWidget::setRegionActual(const QString &region)
{
    m_regionActual = region;
    QPoint pos = m_grafo.posicionRegion(region);
    m_jugadorSprite->setPosition(pos.x(), pos.y());
    update();
}

void MapaWidget::onFrame()
{
    m_jugadorSprite->update(0.016f);
    update();
}

void MapaWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QColor(0, 0, 0, 150));
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    if (!m_imagenMapa.isNull()) {
        int x = (width() - m_imagenMapa.width()) / 2;
        int y = (height() - m_imagenMapa.height()) / 2;
        painter.drawPixmap(x, y, m_imagenMapa);
    }

    if (m_jugadorSprite) {
        m_jugadorSprite->draw(painter);
    }
}

void MapaWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Tab) {
        emit mapaCerrado();
        close();
    }
}
