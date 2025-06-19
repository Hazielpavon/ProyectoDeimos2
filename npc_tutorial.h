#pragma once
#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <vector>

class Jugador;
class TutorialScene; // ✅ nombre correcto

class npc_tutorial : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    npc_tutorial(TutorialScene* level, // ✅ aquí también
                 Jugador* player,
                 QGraphicsScene* scene,
                 const QPointF& pos,
                 QObject* parent = nullptr);

    void update(float dt);

public slots:
    void onBossDefeated();

private:
    enum State { Idle, Talking };
    State m_state = Idle;

    float m_animTimer = 0.0f;
    int m_frameIndex = 0;
    float m_proximity = 50.0f;
    float m_talkCooldown = 1.0f;

    std::vector<QPixmap> m_idleFrames;
    std::vector<QPixmap> m_speakFrames;

    Jugador* m_player;
    TutorialScene* m_level;  // ✅ corregido

    QGraphicsTextItem* m_textItem = nullptr;

    void loadAnimations();
    void talk();
};
