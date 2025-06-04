#pragma once
#include <QPixmap>
#include <QVector>
#include <QMap>
#include <QPoint>
#include <QString>
#include <QSize>

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
    SliddingLeft
    // Faltan mas estados (Este metodo funciona para varios Sprites para todos, faltan un par de modificaciones con las rutas)
};

class Sprite
{
public:
    Sprite();

    // Carga todos los frames para un estado dado:
    void loadFrames(SpriteState state, const QString &prefix, int count);
    void generateMirroredFrames(SpriteState srcState, SpriteState dstState);

    void setPosition(int x, int y);
    const QPoint& getPosition() const { return m_pos; }

    void setSize(int w, int h);
    QSize getSize() const { return m_drawSize; }

    void setState(SpriteState newState);
    void setFPS(int framesPerSecond);

    // Llamar cada frame con dt para avanzar la animación:
    void update(float dt);

    // Dibuja el sprite en un QPainter (como antes):
    void draw(QPainter &painter) const;

    // —> NUEVO: devuelve directamente el pixmap del frame actual, sin escalar:
    QPixmap currentFrame() const;

private:
    int m_frameIndex;
    float m_timeAccumulator;
    float m_secondsPerFrame;
    SpriteState m_state;

    // Para cada SpriteState guardamos todos sus QPixmap:
    QMap<SpriteState, QVector<QPixmap>> m_frames;

    QSize  m_drawSize;
    QPoint m_pos;
};


