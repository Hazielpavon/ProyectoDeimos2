#pragma once
#include <QObject>
#include <QGraphicsPixmapItem>
#include <vector>

class Jugador;
class niveltorredelamarca;

class NPC : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    NPC(niveltorredelamarca* level,
        Jugador* player,
        QGraphicsScene* scene,
        const QPointF& pos,
        QObject* parent=nullptr);

    void update(float dt);
public slots:
    void onBossDefeated();
private:
    enum State { Idle, Talking };
    State                m_state         = Idle;
    float                m_animTimer     = 0.0f;
    int                  m_frameIndex    = 0;
    float                m_proximity     = 50.0f;   // distancia para iniciar diálogo
    bool                 m_hasQuest      = false;
    bool                 m_accepted      = false;
    bool                 m_rewardGiven   = false;
    float    m_talkCooldown = 1.0f;
    std::vector<QPixmap> m_idleFrames;
    std::vector<QPixmap> m_speakFrames;

    Jugador*             m_player;
    niveltorredelamarca* m_level;

    void loadAnimations();
    void talk();
};
