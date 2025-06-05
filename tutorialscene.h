#ifndef TUTORIALSCENE_H
#define TUTORIALSCENE_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include "entidad.h"

class TutorialScene : public QWidget {
    Q_OBJECT

public:
    // Constructor: recibe el puntero a la entidad (jugador) creado desde MainWindow
    explicit TutorialScene(entidad *jugadorPrincipal, QWidget *parent = nullptr);

protected:
    // Capturar eventos de teclado
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event)override;
private slots:
    // Slot que se llamará cada 16 ms para actualizar física, animación y centrar la cámara
    void onFrame();

private:
    // Puntero a la entidad (lógica + animaciones)
    entidad   *m_player;

    // Timer interno para ~60 FPS
    QTimer    *m_timer;

    // Flags de entrada de teclado
    bool       m_moverIzq;         // A presionado
    bool       m_moverDer;         // D presionado
    bool       m_shiftPresionado;  // Shift presionado
    bool       m_saltoSolicitado;  // Espacio presionado

    // Flags para ocultar el cartel “TUTORIAL” tras caminar y saltar
    bool       m_yaCaminó;
    bool       m_yaSaltó;

    // Vista y escena de Qt Graphics
    QGraphicsView   *m_view;
    QGraphicsScene  *m_scene;

    // Ítems en la escena:
    QGraphicsPixmapItem *m_fondoItem;       // Fondo (“templo_silencio.PNG”)
    QGraphicsPixmapItem *m_cartelItem;      // Cartel “tutorial.png”
    QGraphicsPixmapItem *m_jugadorItem;     // Ítem que dibuja el sprite del jugador
    QGraphicsRectItem  *m_plataformaItem;   // Plataforma intermedia (gris)
    QGraphicsRectItem  *m_sueloItem;        // Suelo invisible (colisión)

    // Dimensiones de la ventana
    static constexpr int WINDOW_WIDTH  = 950;
    static constexpr int WINDOW_HEIGHT = 650;

    // Tamaño de la plataforma
    static constexpr int PLAT_WIDTH  = 120;
    static constexpr int PLAT_HEIGHT = 20;

    // Margen “suelo gráfico” dentro del PNG de fondo (40 px desde abajo)
    static constexpr int SUELO_GRAFICO_ALTURA = 40;

    // Delta‐time fijo
    const float m_dt = 0.016f;  // ≈ 60 FPS

    // Variables auxiliares
    int m_limiteSueloCentroY;  // Coordenada Y del “centro” del sprite cuando esté en el suelo

    QGraphicsPixmapItem *m_instruccionCaminarItem;

    QGraphicsPixmapItem *m_instruccionSaltarItem;
    bool m_mostrarSaltarPendiente;
    float m_tiempoParaMostrarSaltar;
    bool m_saltoYaMostrado;

    QGraphicsPixmapItem* m_instruccionCorrerItem;
    bool m_mostrarCorrerPendiente;
    float m_tiempoParaMostrarCorrer;
    bool m_correrYaMostrado;
    bool m_yaCorrió;

    QGraphicsPixmapItem *m_instruccionDashItem;
    bool m_mostrarDashPendiente;
    float m_tiempoParaMostrarDash;
    bool m_dashYaMostrado;
    bool m_yaHizoDash;

    QGraphicsPixmapItem* m_instruccionGolpearItem;
    bool m_mostrarGolpearPendiente;
    float m_tiempoParaMostrarGolpear;
    bool m_golpearYaMostrado;
    bool m_yaGolpeó;
};

#endif // TUTORIALSCENE_H
