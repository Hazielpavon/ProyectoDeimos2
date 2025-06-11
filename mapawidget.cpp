// MapaWidget.cpp (completo con animación de ruta roja)
#include "mapawidget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <QLabel>
#include <QPushButton>

MapaWidget::MapaWidget(const QString &regionInicial, QWidget *parent)
    : QWidget(parent),
    m_jugadorSprite(new Sprite),
    m_timer(new QTimer(this)),
    m_animacionTimer(new QTimer(this)),
    m_regionActual(regionInicial),
    m_animacionIndex(0)
{
    setFixedSize(950, 650);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    m_imagenMapa.load(":/resources/mapa_centrado.png");
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

    connect(m_animacionTimer, &QTimer::timeout, this, &MapaWidget::animarRuta);

    auto crearClickableLabel = [this](const QString &region, const QPoint &mapaPos) {
        QLabel *label = new QLabel(this);
        label->setGeometry(mapaPos.x(), mapaPos.y(), 40, 40);
        label->setStyleSheet("background-color: transparent;");
        label->setCursor(Qt::PointingHandCursor);
        label->setProperty("region", region);
        label->installEventFilter(this);
        m_clickLabels[region] = label;
    };

    crearClickableLabel("Templo del Silencio", QPoint(230, 300));
    crearClickableLabel("Raices Olvidadas", QPoint(240, 445));
    crearClickableLabel("Ciudad Inversa", QPoint(400, 300));
    crearClickableLabel("Torre de la Marca", QPoint(480, 420));
    crearClickableLabel("Máquina del Olvido", QPoint(625, 330));
    crearClickableLabel("Mente Vacía", QPoint(625, 440));

    // Botón borrar ruta
    m_botonBorrarRuta = new QPushButton("Borrar Ruta", this);
    m_botonBorrarRuta->setGeometry(620, 205, 130, 20);
    m_botonBorrarRuta->setStyleSheet(
        "QPushButton { background-color: rgba(140,100,20,220); color: white; font-weight: bold; border-radius: 8px; font-size: 14px; }"
        "QPushButton:hover { background-color: rgba(180,130,30,250); }"
        );
    connect(m_botonBorrarRuta, &QPushButton::clicked, this, [this]() {
        m_rutaActual.clear();
        m_puntosAnimados.clear();
        m_animacionIndex = 0;
        m_animacionTimer->stop();
        update();
    });
}

MapaWidget::~MapaWidget() {
    delete m_jugadorSprite;
}

void MapaWidget::setRegionActual(const QString &region) {
    m_regionActual = region;
    QPoint pos = m_grafo.posicionRegion(region);
    m_jugadorSprite->setPosition(pos.x(), pos.y());
    update();
}

void MapaWidget::setRutaActual(const QList<QString> &ruta) {
    m_rutaActual = ruta;
    m_puntosAnimados.clear();
    m_animacionIndex = 0;

    // Preprocesar todos los puntos de la ruta
    m_puntosRuta.clear();
    for (int i = 0; i < ruta.size() - 1; ++i) {
        QString origen = ruta[i];
        QString destino = ruta[i + 1];
        QVector<QPoint> subRuta = m_grafo.rutaManual(origen, destino);
        if (subRuta.isEmpty()) {
            subRuta.append(m_grafo.posicionRegion(origen));
            subRuta.append(m_grafo.posicionRegion(destino));
        }
        for (const QPoint &p : subRuta) {
            m_puntosRuta.append(p);
        }
    }

    m_animacionTimer->start(60); // cada 20ms agregar un punto
}

void MapaWidget::animarRuta() {
    if (m_animacionIndex < m_puntosRuta.size()) {
        m_puntosAnimados.append(m_puntosRuta[m_animacionIndex]);
        m_animacionIndex++;
        update();
    } else {
        m_animacionTimer->stop();
    }
}

void MapaWidget::onFrame() {
    m_jugadorSprite->update(0.016f);
    update();
}

void MapaWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Fondo translúcido
    painter.setBrush(QColor(0, 0, 0, 150));
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    // Imagen centrada
    int offsetX = (width() - m_imagenMapa.width()) / 2;
    int offsetY = (height() - m_imagenMapa.height()) / 2;
    if (!m_imagenMapa.isNull()) {
        painter.drawPixmap(offsetX, offsetY, m_imagenMapa);
    }

    // Ruta animada (color burdeos elegante)
    if (!m_puntosAnimados.isEmpty()) {
        painter.save();
        painter.translate(offsetX, offsetY);

        QColor colorBurdeos(180, 30, 40, 220);  // Rojo burdeos elegante
        QPen penRuta(colorBurdeos, 6, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(penRuta);

        for (int i = 0; i < m_puntosAnimados.size() - 1; ++i) {
            painter.drawLine(m_puntosAnimados[i], m_puntosAnimados[i + 1]);
        }

        painter.restore();
    }

    // Dibujar personaje
    if (m_jugadorSprite) {
        m_jugadorSprite->draw(painter);
    }
}

void MapaWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_M) {
        emit mapaCerrado();
        close();
    }
}

bool MapaWidget::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QLabel *label = qobject_cast<QLabel*>(obj);
        if (label && label->property("region").isValid()) {
            QString destino = label->property("region").toString();
            QList<QString> ruta = m_grafo.rutaMasCorta(m_regionActual, destino);
            setRutaActual(ruta);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
