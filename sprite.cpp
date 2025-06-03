#include "sprite.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QPainter>
#include <QTransform>  // Para transformar y voltear pixmaps

Sprite::Sprite()
    : m_frameIndex(0)
    , m_timeAccumulator(0.0f)
    , m_secondsPerFrame(1.0f / 12.0f)
    , m_state(SpriteState::Idle)
    , m_drawSize(64, 64)
    , m_pos(0, 0)
{
}

void Sprite::loadFrames(SpriteState state, const QString &prefix, int count)
{
    m_frames[state].clear();
    m_frameIndex = 0;
    m_timeAccumulator = 0.0f;
    const bool esRecurso = prefix.startsWith(":/");

    QString exeDir, projectRoot;
    if (!esRecurso) {
        exeDir      = QCoreApplication::applicationDirPath();
        projectRoot = QDir(exeDir).absoluteFilePath("../..");
    }

    for (int i = 0; i < count; ++i) {
        QString number = QString("%1").arg(i, 3, 10, QChar('0'));

        QString path;
        if (esRecurso) {
            path = prefix + number + ".png";
        } else {
            QString relPath = prefix + number + ".png";
            path = QDir(projectRoot).absoluteFilePath(relPath);
        }
        QPixmap pix(path);
        if (pix.isNull()) {
            qWarning() << "[loadFrames] NO pudo cargar:" << path;
        } else {
            qDebug() << "[loadFrames] Cargó:" << path << "size" << pix.size();
        }
        m_frames[state].append(pix);
    }
}

void Sprite::generateMirroredFrames(SpriteState srcState, SpriteState dstState)
{
    m_frames[dstState].clear();

    if (!m_frames.contains(srcState) || m_frames[srcState].isEmpty()) {
        qWarning() << "[Sprite] Aviso: no hay frames en srcState"
                   << static_cast<int>(srcState)
                   << "para generarlos espejados.";
        return;
    }

    for (const QPixmap &orig : m_frames[srcState]) {
        if (orig.isNull()) {
            m_frames[dstState].append(QPixmap());
        } else {
            QPixmap flipped = orig.transformed(QTransform().scale(-1, 1));
            m_frames[dstState].append(flipped);
        }
    }
    qDebug() << "[Sprite] Generados" << m_frames[dstState].size()
             << "frames espejados de estado" << static_cast<int>(srcState)
             << "hacia" << static_cast<int>(dstState);
}

void Sprite::setPosition(int x, int y)
{
    m_pos.setX(x);
    m_pos.setY(y);
}

void Sprite::setSize(int w, int h)
{
    m_drawSize = QSize(w, h);
}

void Sprite::setState(SpriteState newState)
{
    if (m_state != newState) {
        m_state = newState;
        m_frameIndex = 0;
        m_timeAccumulator = 0.0f;
    }
}

void Sprite::setFPS(int framesPerSecond)
{
    if (framesPerSecond > 0) {
        m_secondsPerFrame = 1.0f / float(framesPerSecond);
    }
}

void Sprite::update(float dt)
{
    const QVector<QPixmap> &currentFrames = m_frames.value(m_state);
    if (currentFrames.isEmpty())
        return;
    m_timeAccumulator += dt;
    while (m_timeAccumulator >= m_secondsPerFrame) {
        m_timeAccumulator -= m_secondsPerFrame;
        m_frameIndex = (m_frameIndex + 1) % currentFrames.size();
    }
}

void Sprite::draw(QPainter &painter) const
{
    const QVector<QPixmap> &currentFrames = m_frames.value(m_state);
    if (currentFrames.isEmpty())
        return;
    const QPixmap &orig = currentFrames.at(m_frameIndex);
    if (orig.isNull())
        return;
    QPixmap scaledPix = orig.scaled(
        m_drawSize.width(),
        m_drawSize.height(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );
    painter.drawPixmap(m_pos.x(), m_pos.y(), scaledPix);
}
