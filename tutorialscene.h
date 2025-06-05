#ifndef TUTORIALSCENE_H
#define TUTORIALSCENE_H
#include <QLabel>
#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include "entidad.h"
#include "mapawidget.h"
class MainWindow; // Forward declaration

// En la clase:

class TutorialScene : public QWidget {
    Q_OBJECT

public:
   explicit TutorialScene(entidad *jugadorPrincipal, MainWindow *mainWindow, QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event)override;
private slots:
    void onFrame();
private:
    MainWindow* m_mainWindow;
    QLabel* m_mapaRegiones;
    QString m_regionActual;
    MapaWidget *m_minimapa;
    entidad   *m_player;
    QTimer    *m_timer;
    bool       m_moverIzq;
    bool       m_moverDer;
    bool       m_shiftPresionado;
    bool       m_saltoSolicitado;
    bool       m_yaCaminó;
    bool       m_yaSaltó;
    QGraphicsView   *m_view;
    QGraphicsScene  *m_scene;
    QGraphicsPixmapItem *m_fondoItem;
    QGraphicsPixmapItem *m_cartelItem;
    QGraphicsPixmapItem *m_jugadorItem;
    QGraphicsRectItem  *m_plataformaItem;
    QGraphicsRectItem  *m_sueloItem;

    static constexpr int WINDOW_WIDTH  = 950;
    static constexpr int WINDOW_HEIGHT = 650;

    static constexpr int PLAT_WIDTH  = 120;
    static constexpr int PLAT_HEIGHT = 20;


    static constexpr int SUELO_GRAFICO_ALTURA = 40;

    // Delta‐time fijo
    const float m_dt = 0.016f;

    int m_limiteSueloCentroY;

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

#endif
