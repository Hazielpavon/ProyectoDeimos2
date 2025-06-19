// ===========================================================
//  NIVELRAICESOLVIDADAS.H — CABECERA COMPLETA (con CombateManager)
// ===========================================================
#pragma once
#include "Demon.h"
#include "drop.h"
#include <QWidget>
#include <QTimer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QVector>
#include "fireball.h"
#include "ObjetosYColisiones.h"
#include "enemigo.h"
#include "entidad.h"
#include "combatemanager.h"      // 👈 gestor de combate
#include <QLabel>  //inventario temporal

class MainWindow;
class MapaWidget;

/* ===========================================================
 *  Clase del nivel “Raíces Olvidadas”
 * =========================================================== */
class NivelRaicesOlvidadas : public QWidget
{
    Q_OBJECT
public:
    explicit NivelRaicesOlvidadas(entidad*   jugador,
                                  MainWindow* mainWindow,
                                  QWidget*   parent = nullptr);
    void lanzarHechizo();

protected:
    void keyPressEvent   (QKeyEvent*  event) override;
    void keyReleaseEvent (QKeyEvent*  event) override;
    void mousePressEvent (QMouseEvent* event) override;

private slots:
    void onFrame();                                 // bucle 60 FPS

private:
    /* ─────────── Gameplay ─────────── */
    Demon* m_boss = nullptr;
    QSet<Enemigo*>  m_deadDrops;
    QVector<QPointF> m_enemySpawnPos;
    bool m_bossDropCreado = false;
    QVector<Drop*> m_drops;
    QGraphicsTextItem* m_manaText;
    bool bossDefeated = false;
    QVector<Fireball*> m_fireballs;
    QGraphicsRectItem* m_debugBossHitbox = nullptr;
    entidad*             m_player          = nullptr;
    MainWindow*          m_mainWindow      = nullptr;
    QPointF              m_spawnPos;
    bool                 m_deathScheduled  = false;
    QGraphicsRectItem* m_bossHpBorder = nullptr;
    QGraphicsRectItem* m_bossHpBar    = nullptr;
    QGraphicsRectItem* m_hudManaBar = nullptr;
    /* Enemigos y combate */
    QVector<Enemigo*>    m_enemigos;                // enemigos vivos
    CombateManager*      m_combate         = nullptr; // 👈 NUEVO

    /* ─────────── Qt helpers ───────── */
    QTimer*              m_timer           = nullptr;
    QGraphicsView*       m_view            = nullptr;
    QGraphicsScene*      m_scene           = nullptr;

    /* ─────────── Elementos gráficos ───────── */
    QGraphicsPixmapItem* m_bg2Item         = nullptr;
    QGraphicsPixmapItem* m_fondoItem       = nullptr;
    QGraphicsPixmapItem* m_cartelItem      = nullptr;
    QGraphicsPixmapItem* m_playerItem      = nullptr;
    QGraphicsRectItem*   m_plataformaItem  = nullptr;
    QGraphicsRectItem*   m_sueloItem       = nullptr;

    /* HUD */
    static constexpr int HUD_W      = 350;
    static constexpr int HUD_H      = 35;
    static constexpr int HUD_MARGIN = 10;
    QGraphicsRectItem*   m_hudBorder  = nullptr;
    QGraphicsRectItem*   m_hudManaBorder = nullptr;
    QGraphicsRectItem*   m_hudBar     = nullptr;
    QGraphicsTextItem*   m_hudText    = nullptr;

    /* ─────────── Colisiones ───────── */
    ObjetosYColisiones*  m_colManager      = nullptr;

    /* ─────────── Flags de input ───── */
    bool  m_moveLeft        = false;
    bool  m_moveRight       = false;
    bool  m_run             = false;
    bool  m_jumpRequested   = false;

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

    QLabel* m_inventario = nullptr; //inventario temporal
    bool    m_invVisible = false; //inventario temporal
};
