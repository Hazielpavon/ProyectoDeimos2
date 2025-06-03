// sprite.h
#pragma once

#include <QPixmap>
#include <QVector>
#include <QMap>
#include <QPoint>
#include <QString>
#include <QSize>
#include <QPainter>
#include <QTransform>

enum class SpriteState {
    Idle,
    IdleLeft,
    Walking,
    WalkingLeft,
    Jump,
    JumpLeft,
    Running,
    RunningLeft,
    Sliding,
    SlidingLeft,
    Slashing,
    Slashingleft,
};

class Sprite
{
public:
    Sprite();

    // Carga una secuencia de PNGs (p. ej. “:/resources/0_Blood_Demon_Walking_000.png”, “…_001.png”, …)
    void loadFrames(SpriteState state, const QString &prefix, int count);

    // Genera la versión volteada horizontalmente de una animación ya cargada
    void generateMirroredFrames(SpriteState srcState, SpriteState dstState);

    // Posicionar el sprite en pantalla
    void setPosition(int x, int y);
    const QPoint &getPosition() const { return m_pos; }

    // Cambiar tamaño de dibujo (anchura / altura)
    void setSize(int w, int h);
    QSize getSize() const { return m_drawSize; }

    // Obtener/Establecer estado actual
    SpriteState getState() const { return m_state; }
    void setState(SpriteState newState);

    // Control de FPS (fotogramas por segundo)
    void setFPS(int framesPerSecond);

    // Avanza el fotograma según dt (en segundos)
    void update(float dt);

    // Dibuja el frame actual en el QPainter
    void draw(QPainter &painter) const;

    // ——————————————————————————————
    // Métodos públicos que permiten leer sin romper encapsulación:
    int  currentFrameIndex() const { return m_frameIndex; }
    int  frameCount(SpriteState s) const
    {
        if (m_frames.contains(s)) {
            return m_frames.value(s).size();
        }
        return 0;
    }
    QVector<QPixmap> framesForState(SpriteState state) const {
        // QMap::value() ya devuelve un QVector<QPixmap> (una copia), o un QVector vacío si no encuentra la clave
        return m_frames.value(state);
    }


    // ——————————————————————————————

private:
    int                m_frameIndex;       // índice actual dentro de la animación
    float              m_timeAccumulator;  // acumulador de tiempo
    float              m_secondsPerFrame;  // segundos que dura cada fotograma
    SpriteState        m_state;            // estado (Idle, Walking, Slashing, etc.)
    QMap<SpriteState, QVector<QPixmap>> m_frames;  // mapeo estado → vector de pixmaps
    QSize              m_drawSize;         // tamaño final al dibujar
    QPoint             m_pos;              // posición de dibujo
};
