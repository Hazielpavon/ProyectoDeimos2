
#pragma once
#include "BringerOfDeath.h"
#include "drop.h"
#include <QWidget>
#include <QTimer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QVector>
#include "Fireball.h"
#include "ObjetosYColisiones.h"
#include "enemigo.h"
#include "entidad.h"
#include "CombateManager.h"
#include "npc_tutorial.h"

class MainWindow;
class MapaWidget;

class TutorialScene : public QWidget
{
    Q_OBJECT
public:
    explicit TutorialScene(entidad*   jugador,
                                  MainWindow* mainWindow,
                                  QWidget*   parent = nullptr);
    void lanzarHechizo();

protected:
    void keyPressEvent   (QKeyEvent*  event) override;
    void keyReleaseEvent (QKeyEvent*  event) override;
    void mousePressEvent (QMouseEvent* event) override;

private slots:
    void onFrame();

private:
     QSet<Enemigo*>  m_deadDrops;
    QVector<QPointF> m_enemySpawnPos;
    BringerOfDeath*       m_boss = nullptr;
    npc_tutorial *m_npc = nullptr;
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
    QVector<Enemigo*>    m_enemigos;
    CombateManager*      m_combate         = nullptr;

    QTimer*              m_timer           = nullptr;
    QGraphicsView*       m_view            = nullptr;
    QGraphicsScene*      m_scene           = nullptr;

    QGraphicsPixmapItem* m_bg2Item         = nullptr;
    QGraphicsPixmapItem* m_fondoItem       = nullptr;
    QGraphicsPixmapItem* m_cartelItem      = nullptr;
    QGraphicsPixmapItem* m_playerItem      = nullptr;
    QGraphicsRectItem*   m_plataformaItem  = nullptr;
    QGraphicsRectItem*   m_sueloItem       = nullptr;


    static constexpr int HUD_W      = 350;
    static constexpr int HUD_H      = 35;
    static constexpr int HUD_MARGIN = 10;
    QGraphicsRectItem*   m_hudBorder  = nullptr;
    QGraphicsRectItem*   m_hudManaBorder = nullptr;
    QGraphicsRectItem*   m_hudBar     = nullptr;
    QGraphicsTextItem*   m_hudText    = nullptr;

    ObjetosYColisiones*  m_colManager      = nullptr;

    bool  m_moveLeft        = false;
    bool  m_moveRight       = false;
    bool  m_run             = false;
    bool  m_jumpRequested   = false;

    float m_dt              = 0.0f;
    int   m_repeatCount     = 1;
    int   m_bgWidth         = 0;
    int   m_bgHeight        = 0;
    bool  m_secondBgShown   = false;
    float m_limiteSueloCentroY = 0.0f;

    MapaWidget*          m_mapaRegiones   = nullptr;
    QString              m_currentRegion;

    enum class FaseTutorial { Caminar, Saltar, Correr,  Dash, Golpear, Fireball,  Mapa,  Terminado };
    FaseTutorial m_faseTutorial = FaseTutorial::Caminar;

    QGraphicsPixmapItem* m_tutorialItem = nullptr;
    bool m_yaCamino = false;
    bool m_yaSalto = false;
    bool m_yaCorrio = false;
    bool m_yaDash = false;
    bool m_yaGolpeo  = false;
    bool m_yaFireball = false;

    struct MovingPlatform {
        QGraphicsPixmapItem* sprite;
        QGraphicsRectItem* hitbox;
        float minY, maxY;
        float speed;
        int dir;
    };

    QVector<MovingPlatform> m_movingPlatforms;
    void actualizarPlataformasMoviles();
};
