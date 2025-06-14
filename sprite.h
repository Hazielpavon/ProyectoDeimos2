#pragma once
#include <QPixmap>
#include <QVector>
#include <QMap>
#include <QPoint>
#include <QString>
#include <QSize>
#include <QPainter>          // 👈 necesario para draw()

/* Estados que ya usabas */
enum class SpriteState {
    Idle,
    IdleLeft,
    Walking,
    WalkingLeft,
    Jump,
    JumpLeft,
    Running,
    RunningLeft,
    Slashing,
    SlashingLeft,
    Slidding,
    SliddingLeft,
    dead,
    deadleft
};

class Sprite
{
public:
    Sprite();

    /* Cargar frames */
    void loadFrames(SpriteState state,
                    const QString& prefix,
                    int count);

    /* Generar frames espejados */
    void generateMirroredFrames(SpriteState srcState,
                                SpriteState dstState);

    /* Posición y tamaño en pantalla */
    void setPosition(int x, int y);
    const QPoint& getPosition() const { return m_pos; }

    void setSize(int w, int h);
    QSize getSize() const { return m_drawSize; }

    /* Control de estado / animación */
    void setState(SpriteState newState);
    SpriteState getState() const { return m_state; }   // 👈 NUEVO GETTER

    void setFPS(int framesPerSecond);
    void update(float dt);

    /* Render */
    void draw(QPainter& painter) const;
    QPixmap currentFrame() const;

private:
    int         m_frameIndex;
    float       m_timeAccumulator;
    float       m_secondsPerFrame;

    SpriteState m_state;
    QMap<SpriteState, QVector<QPixmap>> m_frames;

    QSize  m_drawSize;
    QPoint m_pos;
};
