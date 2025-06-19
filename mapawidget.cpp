#include "mapawidget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <cmath>

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
    m_imagenInformacion.load(":/resources/informacion.png");

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

    QStringList regiones = {
        "Templo del Silencio", "Raices Olvidadas", "Ciudad Inversa",
        "Torre de la Marca", "Máquina del Olvido", "Mente Vacía"
    };
    for (const QString &region : regiones) crearClickableLabel(region);

    m_botonBorrarRuta = new QPushButton("", this);
    m_botonBorrarRuta->setGeometry(830, 520, 80, 20);
    m_botonBorrarRuta->setStyleSheet("QPushButton { background-color: transparent; border: none; }");
    m_botonBorrarRuta->raise();
    m_botonBorrarRuta->setEnabled(true);
    connect(m_botonBorrarRuta, &QPushButton::clicked, this, [this]() {
        m_rutaActual.clear();
        m_puntosRuta.clear();
        m_puntosAnimados.clear();
        m_rutaMasCortaPuntos.clear();
        m_rutasAlternativasPuntos.clear();
        m_animacionIndex = 0;
        m_animacionTimer->stop();
        if (m_rutaInfoLabel) m_rutaInfoLabel->clear();
        update();
    });

    // Botón para mostrar/ocultar imagen de información
    m_botonMostrarImagen = new QPushButton("?", this);
    m_botonMostrarImagen->setGeometry(880, 520, 25, 20);
    m_botonMostrarImagen->setStyleSheet("QPushButton { background-color: black; color: white; border: 1px solid white; }");
    m_botonMostrarImagen->raise();
    m_botonMostrarImagen->setEnabled(true);

    connect(m_botonMostrarImagen, &QPushButton::clicked, this, [this]() {
        if (m_imagenRutaLabel && m_imagenRutaLabel->isVisible()) {
            // Si ya está visible, la ocultamos
            m_imagenRutaLabel->hide();
        } else {
            // Si no existe o está oculta, la mostramos
            if (!m_imagenRutaLabel) {
                m_imagenRutaLabel = new QLabel(this);
                m_imagenRutaLabel->setPixmap(QPixmap(":/resources/ruta_inf.PNG").scaled(300, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                m_imagenRutaLabel->setStyleSheet("background-color: rgba(0, 0, 0, 200); border: 2px solid white;");
                m_imagenRutaLabel->setGeometry(500, 300, 300, 200);
            }
            m_imagenRutaLabel->show();
            m_imagenRutaLabel->raise();
        }
    });

    m_rutaInfoLabel = new QLabel(this);
    m_rutaInfoLabel->setGeometry(770, 110, 160, 200);
    m_rutaInfoLabel->setStyleSheet("color: white; font-size: 14px;");
    m_rutaInfoLabel->setWordWrap(true);
}

MapaWidget::~MapaWidget() {
    delete m_jugadorSprite;
}

void MapaWidget::crearClickableLabel(const QString &region) {
    QPoint punto = m_grafo.posicionRegion(region);
    int desplazamientoIzquierda = 100;
    int offsetX = (width() - m_imagenMapa.width()) / 2 - desplazamientoIzquierda;
    int offsetY = (height() - m_imagenMapa.height()) / 2;

    QLabel *label = new QLabel(this);
    label->setGeometry(offsetX + punto.x() - 20, offsetY + punto.y() - 20, 40, 40);
    label->setStyleSheet("background-color: transparent;");
    label->setCursor(Qt::PointingHandCursor);
    label->setProperty("region", region);
    label->installEventFilter(this);
    m_clickLabels[region] = label;
}

void MapaWidget::setRegionActual(const QString &region) {
    m_regionActual = region;
    QPoint pos = m_grafo.posicionRegion(region);
    m_jugadorSprite->setPosition(pos.x(), pos.y());
    update();
}

void MapaWidget::setRutaActual(const QList<QString> &rutaPrincipal) {
    m_rutaActual = rutaPrincipal;
    m_puntosAnimados.clear();
    m_puntosRuta.clear();
    m_animacionIndex = 0;

    m_rutaMasCortaPuntos.clear();
    m_rutasAlternativasPuntos.clear();

    QString destino = rutaPrincipal.last();
QList<QList<QString>> todas = m_grafo.todasLasRutas(m_regionActual, destino);

    QString infoTexto;
    double distanciaMasCorta = std::numeric_limits<double>::max();
    QVector<QPoint> rutaMasCortaTemp;

    for (int i = 0; i < todas.size(); ++i) {
        const QList<QString>& ruta = todas[i];
        QVector<QPoint> puntosRuta;
        double distanciaRuta = 0.0;
        bool rutaValida = true;

        for (int j = 0; j < ruta.size() - 1; ++j) {
            QString origen = ruta[j];
            QString destino = ruta[j + 1];

            QVector<QPoint> subRuta = m_grafo.rutaManual(origen, destino);

            // VALIDACIÓN FINAL: no deben ser menos de 2 puntos
            if (subRuta.size() < 2) {
                qWarning() << "❌ Subruta entre" << origen << "y" << destino << "no tiene puntos suficientes.";
                rutaValida = false;
                break;
            }

            for (const QPoint &p : subRuta)
                puntosRuta.append(p);

            // Medir distancia solo si la subruta es válida
            for (int s = 0; s < subRuta.size() - 1; ++s) {
                QPoint p1 = subRuta[s];
                QPoint p2 = subRuta[s + 1];
                distanciaRuta += std::hypot(p2.x() - p1.x(), p2.y() - p1.y());
            }
        }

        if (!rutaValida)
            continue;

        double distanciaKm = distanciaRuta * 0.01;

        // Mostrar en label
        for (const QString &nombre : ruta)
            infoTexto += nombre + " → ";
        infoTexto.chop(3);
        infoTexto += QString("\nTotal: %1 km\n\n").arg(QString::number(distanciaKm, 'f', 2));

        if (distanciaKm < distanciaMasCorta) {
            if (!rutaMasCortaTemp.isEmpty()) {
                m_rutasAlternativasPuntos.append(rutaMasCortaTemp); // anterior pasa a alternativas
            }
            distanciaMasCorta = distanciaKm;
            rutaMasCortaTemp = puntosRuta;
        } else {
            m_rutasAlternativasPuntos.append(puntosRuta);
        }
    }

    m_rutaMasCortaPuntos = rutaMasCortaTemp;

    if (m_rutaInfoLabel)
        m_rutaInfoLabel->setText(infoTexto.trimmed());

    update();
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
    painter.setBrush(QColor(0, 0, 0, 150));
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    int desplazamientoIzquierda = 100;
    int offsetX = (width() - m_imagenMapa.width()) / 2 - desplazamientoIzquierda;
    int offsetY = (height() - m_imagenMapa.height()) / 2;

    if (!m_imagenMapa.isNull())
        painter.drawPixmap(offsetX, offsetY, m_imagenMapa);

    if (!m_imagenInformacion.isNull()) {
        int infoX = offsetX + m_imagenMapa.width() + 20;
        int infoY = offsetY + (m_imagenMapa.height() - m_imagenInformacion.height()) / 2;
        painter.drawPixmap(infoX, infoY, m_imagenInformacion);
    }

    // ─── RUTA MÁS CORTA ─────────────
    if (m_rutaMasCortaPuntos.size() >= 2) {
        painter.save();
        painter.translate(offsetX, offsetY);

        QPen penVerde(QColor(20, 200, 120, 230), 6, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(penVerde);
        for (int i = 0; i < m_rutaMasCortaPuntos.size() - 1; ++i)
            painter.drawLine(m_rutaMasCortaPuntos[i], m_rutaMasCortaPuntos[i + 1]);

        // 🔴 DEBUG: puntos reales como círculos rojos
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 60, 60, 220));
        for (const QPoint &p : m_rutaMasCortaPuntos)
            painter.drawEllipse(p, 4, 4);  // radio del punto

        painter.restore();
    }

    // ─── RUTAS ALTERNATIVAS ─────────────
    if (!m_rutasAlternativasPuntos.isEmpty()) {
        painter.save();
        painter.translate(offsetX, offsetY);

        QPen penRojo(QColor(220, 50, 50, 120), 4, Qt::DashLine, Qt::RoundCap);
        painter.setPen(penRojo);
        for (const QVector<QPoint> &ruta : m_rutasAlternativasPuntos) {
            if (ruta.size() >= 2) {
                for (int i = 0; i < ruta.size() - 1; ++i)
                    painter.drawLine(ruta[i], ruta[i + 1]);
            }
        }

        // 🔴 DEBUG: puntos de rutas alternativas
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 60, 60, 220));
        for (const QVector<QPoint> &ruta : m_rutasAlternativasPuntos)
            for (const QPoint &p : ruta)
                painter.drawEllipse(p, 4, 4);

        painter.restore();
    }

    // ─── Dibujar DISTANCIAS sobre líneas (solo ruta más corta) ───
    painter.setFont(QFont("Arial", 10));
    painter.setPen(QColor(255, 255, 255));

    for (int i = 0; i < m_rutaActual.size() - 1; ++i) {
        QString origen = m_rutaActual[i];
        QString destino = m_rutaActual[i + 1];
        auto clave = qMakePair(origen, destino);

        if (!m_grafo.obtenerRutasManuales().contains(clave))
            continue;

        QVector<QPoint> ruta = m_grafo.rutaManual(origen, destino);
        if (ruta.size() < 2)
            continue;

        // Punto medio visual (aproximado)
        QPoint puntoInicio = ruta.first();
        QPoint puntoFin = ruta.last();
        QPoint centro = (puntoInicio + puntoFin) / 2;

        double km = m_grafo.obtenerDistanciaVisual(origen, destino);
        QString texto = QString("%1 km").arg(QString::number(km, 'f', 1));

        painter.drawText(centro + QPoint(0, -10), texto);  // pequeño desplazamiento hacia arriba
    }

    // ─── SPRITE DEL JUGADOR ─────────────
    if (m_jugadorSprite)
        m_jugadorSprite->draw(painter);
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




