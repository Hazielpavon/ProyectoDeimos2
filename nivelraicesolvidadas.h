#ifndef NIVELRAICESOLVIDADAS_H
#define NIVELRAICESOLVIDADAS_H
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include "entidad.h"
#include <QTimer>
#include <QKeyEvent>

class NivelRaicesOlvidadas : public QWidget
{
    Q_OBJECT
public:
    explicit NivelRaicesOlvidadas(entidad *jugadorPrincipal, QWidget *parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
private slots:
    void onFrame();
private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    int fondoAncho;
    int fondoAlto;
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
    const float m_dt = 0.016f;
    int m_limiteSueloCentroY;
};

#endif
