#pragma once
#include <QWidget>
#include <QTimer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QVector>

#include "ObjetosYColisiones.h"
<<<<<<< HEAD
#include "Enemigo.h"
#include "CombateManager.h"
=======
#include "enemigo.h"
#include "entidad.h"
#include "CombateManager.h"      // 👈 gestor de combate
>>>>>>> ana

class MainWindow;
class MapaWidget;

/* ────────────────────────────────────────────── */
/*  Struct local para plataformas móviles          */
struct InvMovingPlatform
{
    QGraphicsPixmapItem* sprite = nullptr;
    QGraphicsRectItem*   hitbox = nullptr;
    float  minX  = 0.f;
    float  maxX  = 0.f;
    float  speed = 0.f;
    int    dir   = +1;     // +1 → derecha, -1 → izquierda
};
/* ────────────────────────────────────────────── */

class ciudadinversa : public QWidget
{
    Q_OBJECT
public:
    explicit ciudadinversa(entidad* jugador,
                           MainWindow* mainWindow,
                           QWidget* parent = nullptr);

protected:
    void keyPressEvent   (QKeyEvent*)   override;
    void keyReleaseEvent (QKeyEvent*)   override;
    void mousePressEvent (QMouseEvent*) override;

private slots:
    void onFrame();

private:
    /* ---------- Gameplay ---------- */
    entidad*          m_player        = nullptr;
    MainWindow*       m_mainWindow    = nullptr;
    QPointF           m_spawnPos;
    bool              m_deathScheduled = false;

    QVector<InvMovingPlatform> m_movingPlatforms;
    QVector<Enemigo*>   m_enemigos;
    CombateManager*     m_combate       = nullptr;

    /* ---------- Qt helpers ---------- */
    QTimer*            m_timer         = nullptr;
    QGraphicsView*     m_view          = nullptr;
    QGraphicsScene*    m_scene         = nullptr;
    ObjetosYColisiones* m_colManager   = nullptr;   // ← de nuevo presente

    /* ---------- Elementos gráficos ---------- */
    QGraphicsPixmapItem* m_playerItem      = nullptr;
    QGraphicsPixmapItem* m_bg2Item         = nullptr;
    QGraphicsRectItem*   m_debugBossHitbox = nullptr;
    QGraphicsRectItem*   m_bossHpBorder    = nullptr;
    QGraphicsRectItem*   m_bossHpBar       = nullptr;

    /* HUD */
    static constexpr int HUD_W      = 350;
    static constexpr int HUD_H      = 35;
    static constexpr int HUD_MARGIN = 10;
    QGraphicsRectItem*   m_hudBorder = nullptr;
    QGraphicsRectItem*   m_hudBar    = nullptr;
    QGraphicsTextItem*   m_hudText   = nullptr;

    /* Lógica general */
    float m_dt            = 0.0f;
    int   m_bgWidth       = 0;
    int   m_bgHeight      = 0;
    bool  m_secondBgShown = false;

    /* Entrada */
    bool m_moveLeft = false, m_moveRight = false;
    bool m_run      = false, m_jumpRequested = false;

<<<<<<< HEAD
    /* UI extra */
    MapaWidget*      m_mapaRegiones = nullptr;
    QString          m_currentRegion;
=======
    /* ─────────── Lógica general ───── */
    float m_dt              = 0.0f;
    int   m_repeatCount     = 1;
    int   m_bgWidth         = 0;
    int   m_bgHeight        = 0;
    bool  m_secondBgShown   = false;
    float m_limiteSueloCentroY = 0.0f;

    /* ─────────── UI extra ─────────── */
    MapaWidget*          m_mapaRegiones   = nullptr;
    QString              m_currentRegion;

        bool m_inverted = true;

>>>>>>> ana
};
