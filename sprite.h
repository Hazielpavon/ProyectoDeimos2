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
    Attacking,
    WalkingLeft,
    Jump,
    JumpLeft,
    Running,
    RunningLeft,
   // Faltan mas estados (Este metodo funciona para varios Sprites para todos, faltan un par de modificaciones con las rutas)
};

class Sprite
{
public:
    Sprite();

    // Ahora recibe también el estado al que pertenecen estos frames:
    void loadFrames(SpriteState state, const QString &prefix, int count);

    void generateMirroredFrames(SpriteState srcState, SpriteState dstState);

    void setPosition(int x, int y);
    const QPoint &getPosition() const { return m_pos; }

    void setSize(int w, int h);
    QSize getSize() const { return m_drawSize; }

    void setState(SpriteState newState);
    void setFPS(int framesPerSecond);

    void update(float dt);
    void draw(QPainter &painter) const;

    void loadFrames(const QString &prefix, int count);
    \
private:
    int m_frameIndex;
    float m_timeAccumulator;
    float m_secondsPerFrame;
    SpriteState m_state;

    // Contenedor genérico para TODAS las animaciones:
    QMap<SpriteState, QVector<QPixmap>> m_frames;

    QSize m_drawSize;
    QPoint m_pos;  // tamañ // estado actual si manejas varias animaciones
};


