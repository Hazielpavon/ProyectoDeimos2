#ifndef TUTORIALSCENE_H
#define TUTORIALSCENE_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QTimer>

class TutorialScene : public QWidget {
    Q_OBJECT

public:
    explicit TutorialScene(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void actualizarMovimiento();

private:
    // Escena y vista
    QGraphicsScene *scene;
    QGraphicsView *view;

    // Elementos del juego
    QGraphicsEllipseItem *jugador;
    QGraphicsRectItem *piso;
    QGraphicsRectItem *plataforma;

    // Imagen del tutorial
    QGraphicsPixmapItem *imagenTutorial;

    // Timer de animación
    QTimer *timer;

    // Movimiento
    bool moverIzquierda = false;
    bool moverDerecha = false;

    // Física
    qreal velocidadY;
    qreal gravedad;
    qreal limiteSueloY;

    // Fondo
    int fondoAncho;
    int fondoAlto;

    // Estado del tutorial
    bool mensajeMostrado = false;
    bool mensajeSaltoMostrado = false;
    bool jugadorYaSeMovio = false;
    bool saltoHabilitado = false;
    bool yaSalto = false;
};

#endif // TUTORIALSCENE_H
