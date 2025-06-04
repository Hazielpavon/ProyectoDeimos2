#include "tutorialscene.h"
#include <QPixmap>
#include <QVBoxLayout>
#include <QGraphicsTextItem>

TutorialScene::TutorialScene(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(950, 650);

    QPixmap fondo(":/resources/templo_silencio.PNG");
    fondoAncho = fondo.width();
    fondoAlto = fondo.height();
    const int repeticiones = 3;

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, fondoAncho * repeticiones, fondoAlto);

    for (int i = 0; i < repeticiones; ++i) {
        QGraphicsPixmapItem *fondoItem = new QGraphicsPixmapItem(fondo);
        fondoItem->setPos(i * fondoAncho, 0);
        fondoItem->setZValue(0);
        scene->addItem(fondoItem);
    }

    piso = new QGraphicsRectItem(0, fondoAlto - 40, fondoAncho * repeticiones, 40);
    piso->setBrush(Qt::transparent);
    piso->setPen(Qt::NoPen);
    piso->setZValue(1);
    scene->addItem(piso);

    plataforma = new QGraphicsRectItem(0, 0, 120, 20);
    plataforma->setBrush(Qt::darkGray);
    plataforma->setZValue(1);
    plataforma->setPos(300, fondoAlto - 160);
    scene->addItem(plataforma);

    jugador = new QGraphicsEllipseItem(0, 0, 40, 40);
    jugador->setBrush(Qt::red);
    jugador->setZValue(2);
    jugador->setPos(100, -300);
    scene->addItem(jugador);

    limiteSueloY = fondoAlto - 80;

    view = new QGraphicsView(scene, this);
    view->setFixedSize(950, 650);
    view->centerOn(jugador);
    view->scale(0.9, 0.9);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::StrongFocus);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(view);
    setLayout(layout);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TutorialScene::actualizarMovimiento);
    timer->start(16);

    velocidadY = 0.0;
    gravedad = 0.7;

    mensajeMostrado = false;
    jugadorYaSeMovio = false;
    saltoHabilitado = false;
    yaSalto = false;
    mensajeSaltoMostrado = false;

    imagenTutorial = nullptr;
}

void TutorialScene::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_A)
        moverIzquierda = true;
    else if (event->key() == Qt::Key_D)
        moverDerecha = true;
    else if (event->key() == Qt::Key_Space && saltoHabilitado) {
        velocidadY = -15;
        yaSalto = true;
    }
}

void TutorialScene::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_A)
        moverIzquierda = false;
    else if (event->key() == Qt::Key_D)
        moverDerecha = false;
}

void TutorialScene::actualizarMovimiento()
{
    int dx = 0;
    if (moverIzquierda || moverDerecha)
        jugadorYaSeMovio = true;

    if (moverIzquierda) dx = -5;
    if (moverDerecha) dx = 5;

    qreal x = jugador->x();
    qreal y = jugador->y();

    velocidadY += gravedad;
    y += velocidadY;

    bool tocandoSuelo = false;

    if (y >= limiteSueloY) {
        y = limiteSueloY;
        velocidadY = 0;
        saltoHabilitado = true;
        tocandoSuelo = true;
    }

    QRectF jugadorRect(x + dx, y, jugador->rect().width(), jugador->rect().height());
    QRectF plataformaRect = plataforma->sceneBoundingRect();

    if (jugadorRect.intersects(plataformaRect)) {
        qreal jugadorAbajo = y + jugador->rect().height();
        qreal plataformaArriba = plataforma->y();

        if (jugadorAbajo >= plataformaArriba && velocidadY >= 0) {
            y = plataformaArriba - jugador->rect().height();
            velocidadY = 0;
            saltoHabilitado = true;
            tocandoSuelo = true;
        }
    }

    jugador->setPos(x + dx, y);
    view->centerOn(jugador);

    if (tocandoSuelo && !mensajeMostrado) {
        mensajeMostrado = true;

        QPixmap cartel(":/resources/tutorial.png");
        imagenTutorial = new QGraphicsPixmapItem(cartel);
        imagenTutorial->setZValue(10);

        qreal posX = x + jugador->rect().width() / 2 - cartel.width() / 2;
        qreal posY = y - cartel.height() - 30;

        imagenTutorial->setPos(posX, posY);
        scene->addItem(imagenTutorial);
    }

    // ✅ Solo quitar imagen si ya caminó Y ya saltó
    if (imagenTutorial && jugadorYaSeMovio && yaSalto) {
        scene->removeItem(imagenTutorial);
        delete imagenTutorial;
        imagenTutorial = nullptr;
    }
}
