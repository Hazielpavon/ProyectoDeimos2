// nivelraicesolvidadas.h
#pragma once

#include <QWidget>
#include <QTimer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include "entidad.h"
#include "ObjetosYColisiones.h"

class MainWindow;

class NivelRaicesOlvidadas : public QWidget
{
    Q_OBJECT
public:
    explicit NivelRaicesOlvidadas(entidad* jugador,
                                  MainWindow* mainWindow,
                                  QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void onFrame();

private:
    entidad*              m_player;
    MainWindow*           m_mainWindow;
    QTimer*               m_timer;
    QGraphicsView*        m_view;
    QGraphicsScene*       m_scene;
    ObjetosYColisiones*   m_colManager;
    QLabel*               m_mapaRegiones;
    QGraphicsPixmapItem*  m_playerItem;

    bool                  m_moveLeft;
    bool                  m_moveRight;
    bool                  m_run;
    bool                  m_jumpRequested;

    float                 m_dt;
    int                   m_repeatCount;
    int                   m_bgWidth;
    int                   m_bgHeight;
    bool                  m_secondBgShown;
    QString               m_currentRegion;
};
