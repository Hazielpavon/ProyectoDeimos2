#pragma once
#include <QWidget>
#include <QTimer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include "ObjetosYColisiones.h"
#include "entidad.h"

class MainWindow;
class MapaWidget;

class niveltorredelamarca : public QWidget
{
    Q_OBJECT
public:
    niveltorredelamarca(entidad* jugador,MainWindow* mainWindow,QWidget* parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void onFrame();
private:
    QPointF m_spawnPos;
    bool m_deathScheduled = false;
    QGraphicsPixmapItem* m_bg2Item;
    static constexpr int HUD_W      = 350;
    static constexpr int HUD_H      = 35;
    static constexpr int HUD_MARGIN = 10;
    QGraphicsRectItem* m_hudBorder = nullptr;
    QGraphicsRectItem* m_hudBar    = nullptr;
    QGraphicsTextItem* m_hudText   = nullptr;
    entidad*             m_player;
    MainWindow*          m_mainWindow;
    QTimer*              m_timer;
    QGraphicsView*       m_view;
    QGraphicsScene*      m_scene;
    ObjetosYColisiones*  m_colManager;
    QGraphicsPixmapItem* m_playerItem;
    QGraphicsRectItem*   m_debugBox;
    bool m_moveLeft, m_moveRight, m_run, m_jumpRequested;
    float m_dt;
    int   m_repeatCount;
    int   m_bgWidth, m_bgHeight;
    bool  m_secondBgShown;
    MapaWidget*         m_mapaRegiones;
    QString             m_currentRegion;
};
