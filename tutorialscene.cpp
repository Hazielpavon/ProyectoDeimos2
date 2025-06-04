#include "tutorialscene.h"
#include <QPainter>
#include <QDebug>

TutorialScene::TutorialScene(entidad *jugadorPrincipal, QWidget *parent)
    : QWidget(parent),
    m_player(jugadorPrincipal),
    m_timer(new QTimer(this)),
    m_moverIzq(false),
    m_moverDer(false),
    m_shiftPresionado(false),
    m_saltoSolicitado(false),
    m_yaCaminó(false),
    m_yaSaltó(false),
    m_view(nullptr),
    m_scene(nullptr),
    m_fondoItem(nullptr),
    m_cartelItem(nullptr),
    m_jugadorItem(nullptr),
    m_plataformaItem(nullptr),
    m_sueloItem(nullptr),
    m_limiteSueloCentroY(0)
{
    // 1) Fijar tamaño del widget a 950×650 (no cambia)
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // 2) CARGAR fondo y calcular su tamaño real
    QPixmap pixFondo(":/resources/templo_silencio.PNG");
    if (pixFondo.isNull()) {
        qWarning() << "[TutorialScene] Error al cargar ':/resources/templo_silencio.PNG'";
    }
    int fondoW = pixFondo.width();   // ej: 900
    int fondoH = pixFondo.height();  // ej: 900
    const int repeticiones = 3;

    // 3) Crear la escena con altura EXACTA del PNG (fondoH), no 650
    m_scene = new QGraphicsScene(0, 0, fondoW * repeticiones, fondoH, this);

    // 4) Añadir N copies del fondo, cada una en Z = 0
    for (int i = 0; i < repeticiones; ++i) {
        QGraphicsPixmapItem *itemFondo = new QGraphicsPixmapItem(pixFondo);
        itemFondo->setZValue(0);
        itemFondo->setPos(i * fondoW, 0);
        m_scene->addItem(itemFondo);
    }

    // 5) AÑADIR plataforma intermedia en Z = 1
    //    → “suelo gráfico” está en y = fondoH - 40
    //    → queremos plataforma 160px encima de ese suelo
    //    → topPlatY = (fondoH - 40) - 160
    int sueloGraficoY = fondoH - 40;
    int topPlatY      = sueloGraficoY - 160;
    m_plataformaItem = new QGraphicsRectItem(0, 0, PLAT_WIDTH, PLAT_HEIGHT);
    m_plataformaItem->setBrush(QColor(80, 80, 80));
    m_plataformaItem->setPen(Qt::NoPen);
    m_plataformaItem->setZValue(1);
    // Moverla a X=300, Y=topPlatY
    m_plataformaItem->setPos(300, topPlatY);
    m_scene->addItem(m_plataformaItem);

    // 6) AÑADIR “suelo invisible” en Z = 2 (para colisión)
    //    → El suelo invisible debe estar en y = fondoH - 40 (solo colisión)
    m_sueloItem = new QGraphicsRectItem(0,
                                        fondoH - 40,
                                        fondoW * repeticiones,
                                        40);
    m_sueloItem->setPen(Qt::NoPen);
    m_sueloItem->setBrush(Qt::NoBrush);
    m_sueloItem->setZValue(2);
    m_scene->addItem(m_sueloItem);

    // 7) CALCULAR m_limiteSueloCentroY en función de fondoH, no 650
    if (m_player) {
        QSize tamSprite = m_player->sprite().getSize();
        // Queremos que los “pies” del sprite estén en (fondoH - 40).
        // Si las “patas” del sprite son (centerY + tamSprite.height()/2), entonces:
        // centerY = (fondoH - 40) - (tamSprite.height()/2)
        m_limiteSueloCentroY = (fondoH - 40) - (tamSprite.height() / 2);
    } else {
        // Si por alguna razón m_player es nulo, hacemos un valor aproximado:
        m_limiteSueloCentroY = (fondoH - 40) - (128 / 2);
    }

    // 8) POSICIONAR al jugador sobre ese suelo invisible por primera vez:
    if (m_player) {
        QPointF posIni = m_player->transform().getPosition();
        // Forzar centroY al suelo
        m_player->transform().setPosition(posIni.x(), m_limiteSueloCentroY);

        // Sincronizar la posición del sprite en pantalla:
        QSize tamSpr = m_player->sprite().getSize();
        int drawX = int(posIni.x() - (tamSpr.width()  / 2.0f));
        int drawY = int(m_limiteSueloCentroY   - (tamSpr.height() / 2.0f));
        m_player->sprite().setPosition(drawX, drawY);
    }

    // 9) AÑADIR ítem para el sprite del jugador en Z = 3
    m_jugadorItem = new QGraphicsPixmapItem();
    m_jugadorItem->setZValue(3);
    m_scene->addItem(m_jugadorItem);

    // Pintar el frame inicial (Idle) del sprite, si existe
    if (m_player) {
        QPixmap frameIni = m_player->sprite().currentFrame();
        QSize   tamSpr   = m_player->sprite().getSize();
        if (!frameIni.isNull()) {
            QPixmap escala = frameIni.scaled(
                tamSpr.width(), tamSpr.height(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
            m_jugadorItem->setPixmap(escala);
            // Posicionar el ítem:
            int drawX = int(m_player->transform().getPosition().x() - (tamSpr.width()  / 2.0f));
            int drawY = int(m_player->transform().getPosition().y() - (tamSpr.height() / 2.0f));
            m_jugadorItem->setPos(drawX, drawY);
        }
    }

    // 10) AÑADIR cartel “TUTORIAL” en Z = 4
    QPixmap pixCartel(":/resources/tutorial.png");
    if (pixCartel.isNull()) {
        qWarning() << "[TutorialScene] Error al cargar ':/resources/tutorial.png'";
        m_cartelItem = nullptr;
    } else {
        m_cartelItem = new QGraphicsPixmapItem(pixCartel);
        m_cartelItem->setZValue(4);
        m_cartelItem->setPos(20, 20);
        m_scene->addItem(m_cartelItem);
    }

    // 11) CREAR QGraphicsView de 950×650 y asignarle la escena
    m_view = new QGraphicsView(m_scene, this);
    m_view->setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->move(0, 0);

    // 12) Conectar el timer interno y arrancarlo (~60 FPS)
    connect(m_timer, &QTimer::timeout, this, &TutorialScene::onFrame);
    m_timer->start(int(m_dt * 1000));

    // 13) El widget debe recibir foco para capturar teclas
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // 14) Centrar la vista EN EL LUGAR CORRECTO: justo en el jugador
    //     De ese modo, el “suelo gráfico” (a y = fondoH - 40) se verá dentro de los 650 px
    if (m_jugadorItem) {
        m_view->centerOn(m_jugadorItem);
    }

    qDebug() << "[TutorialScene] Jugador inicial centrado en Y =" << m_limiteSueloCentroY;
}

// Captura de teclas: A, D, Shift y Espacio
void TutorialScene::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A:
        m_moverIzq = true;
        break;
    case Qt::Key_D:
        m_moverDer = true;
        break;
    case Qt::Key_Shift:
        m_shiftPresionado = true;
        break;
    case Qt::Key_Space:
        m_saltoSolicitado = true;
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

// Suelta de teclas: A, D y Shift
void TutorialScene::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_A:
        m_moverIzq = false;
        break;
    case Qt::Key_D:
        m_moverDer = false;
        break;
    case Qt::Key_Shift:
        m_shiftPresionado = false;
        break;
    default:
        QWidget::keyReleaseEvent(event);
    }
}

// -----------------------------------------------------------------------------
// onFrame(): se llama cada 16 ms por el QTimer interno.
//   1) Leer A/D/Shift para velocidad horizontal
//   2) Leer Espacio para salto
//   3) Llamar a entidad::actualizar(dt) → aplica gravedad y avanza animación
//   4) Colisiones con plataforma y suelo
//   5) Actualizar pixmap del jugador y su posición
//   6) Hacer centerOn(...) para que la cámara siga al jugador
// -----------------------------------------------------------------------------
void TutorialScene::onFrame()
{
    if (!m_player) return;

    // ───────────────────────────────────────────────────────────────
    // (1) Movimiento horizontal (A / D) y SHIFT para correr
    // ───────────────────────────────────────────────────────────────
    float velocidadBase = 160.0f;
    float vx = 0.0f;
    if (m_moverIzq) {
        vx         = -velocidadBase;
        m_yaCaminó = true;
    }
    else if (m_moverDer) {
        vx         = +velocidadBase;
        m_yaCaminó = true;
    }
    // Si SHIFT está presionado, duplicamos velocidad (correr)
    if (m_shiftPresionado && vx != 0.0f) {
        vx *= 2.0f; // 320 px/s
    }
    // Conservamos la componente vertical de la velocidad (por si está saltando)
    QPointF velActual = m_player->fisica().velocity();
    m_player->fisica().setVelocity(vx, velActual.y());

    // ───────────────────────────────────────────────────────────────
    // (2) Salto (Espacio) — solo si el jugador está en el suelo invisible
    // ───────────────────────────────────────────────────────────────
    float yCentroActual = m_player->transform().getPosition().y();
    if (m_saltoSolicitado) {
        // Solo saltar si el centro Y del jugador coincide (aprox.) con el suelo invisible
        if (qAbs(yCentroActual - m_limiteSueloCentroY) < 1.0f) {
            m_player->startJump();
            m_yaSaltó = true;
            qDebug() << "[TutorialScene] SALTO ejecutado";
        }
    }
    m_saltoSolicitado = false;

    // ───────────────────────────────────────────────────────────────
    // (3) Actualizar física y animación: entidad::actualizar(dt)
    // ───────────────────────────────────────────────────────────────
    m_player->actualizar(m_dt);

    // ───────────────────────────────────────────────────────────────
    // (4) Colisiones: plataforma intermedia + suelo invisible
    // ───────────────────────────────────────────────────────────────
    {
        // Posición actual del centro del jugador
        QPointF posJugador = m_player->transform().getPosition();
        QSize   tamSpr     = m_player->sprite().getSize();

        // 4a) Colisión con plataforma intermedia
        QRectF rectJugador(
            posJugador.x() - tamSpr.width() / 2.0f,
            posJugador.y() - tamSpr.height() / 2.0f,
            tamSpr.width(),
            tamSpr.height()
            );
        QRectF rectPlataforma = m_plataformaItem->sceneBoundingRect();

        if (rectJugador.intersects(rectPlataforma)) {
            qreal pieJugador = rectJugador.y() + rectJugador.height();
            qreal topPlat    = rectPlataforma.y();
            // Si el jugador está descendiendo y sus pies tocan la plataforma:
            if (m_player->fisica().velocity().y() >= 0.0f && pieJugador >= topPlat) {
                // Ajustar el centro Y para que los pies queden en topPlat
                qreal nuevaY = topPlat - (tamSpr.height() / 2.0f);
                m_player->transform().setPosition(posJugador.x(), nuevaY);
                // Detener la velocidad vertical
                m_player->fisica().setVelocity(
                    m_player->fisica().velocity().x(),
                    0.0f
                    );
                qDebug() << "[TutorialScene] Colisión PLATAFORMA: centroY ->" << nuevaY;
            }
        }

        // 4b) Colisión con el suelo invisible
        if (m_player->transform().getPosition().y() > m_limiteSueloCentroY) {
            m_player->transform().setPosition(posJugador.x(), m_limiteSueloCentroY);
            m_player->fisica().setVelocity(
                m_player->fisica().velocity().x(),
                0.0f
                );
            qDebug() << "[TutorialScene] Colisión SUELO: centroY ->" << m_limiteSueloCentroY;
        }
    }

    // ───────────────────────────────────────────────────────────────
    // (5) Actualizar el pixmap del jugador y su posición en escena
    // ───────────────────────────────────────────────────────────────
    {
        // Obtenemos el frame actual del sprite (QPixmap)
        QPixmap frameActual = m_player->sprite().currentFrame();
        QSize   tamSpr      = m_player->sprite().getSize();

        if (!frameActual.isNull()) {
            // Lo escalamos a (tamSpr.width(), tamSpr.height())
            QPixmap scaled = frameActual.scaled(
                tamSpr.width(),
                tamSpr.height(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
            m_jugadorItem->setPixmap(scaled);
        }

        // Reposicionamos el ítem para que esté centrado en transform().getPosition()
        QPointF posFinal = m_player->transform().getPosition();
        int drawX = int(posFinal.x() - (tamSpr.width()  / 2.0f));
        int drawY = int(posFinal.y() - (tamSpr.height() / 2.0f));
        m_jugadorItem->setPos(drawX, drawY);
    }

    // ───────────────────────────────────────────────────────────────
    // (6) Centrar la cámara en el jugador
    // ───────────────────────────────────────────────────────────────
    // Hacemos que el QGraphicsView centre su vista en el boundingRect del ítem del jugador:
    if (m_view && m_jugadorItem) {
        m_view->centerOn(m_jugadorItem);
    }

    // ───────────────────────────────────────────────────────────────
    // (7) Ocultar el cartel “TUTORIAL” en cuanto el jugador ya haya
    //     caminado (A/D) y saltado (Espacio) al menos una vez:
    // ───────────────────────────────────────────────────────────────
    if (m_cartelItem && (m_yaCaminó && m_yaSaltó)) {
        // Removemos el ítem de la escena y lo borramos
        m_scene->removeItem(m_cartelItem);
        delete m_cartelItem;
        m_cartelItem = nullptr;
        qDebug() << "[TutorialScene] Cartel eliminado (ya caminó y saltó).";
    }
}

// -----------------------------------------------------------------------------
// NOTA: ya no usamos paintEvent() para dibujar, porque el QGraphicsView
// se encarga de pintar la escena completa. Asegúrate de que NO tengas override
// de paintEvent aquí, o si lo tienes déjalo vacío.
// -----------------------------------------------------------------------------
