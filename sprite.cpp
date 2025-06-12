#include "sprite.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QTransform>

/* ---------- Constructor ---------- */
Sprite::Sprite()
    : m_frameIndex(0)
    , m_timeAccumulator(0.0f)
    , m_secondsPerFrame(1.0f / 12.0f)   // 12 fps por defecto
    , m_state(SpriteState::Idle)
    , m_drawSize(64, 64)
    , m_pos(0, 0)
{}

/* ---------- Carga de frames ---------- */
void Sprite::loadFrames(SpriteState state,
                        const QString& prefix,
                        int count)
{
    m_frames[state].clear();
    m_frameIndex      = 0;
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
        if (esRecurso)
            path = prefix + number + ".png";
        else
            path = QDir(projectRoot).absoluteFilePath(prefix + number + ".png");

        QPixmap pix(path);
        if (pix.isNull())
            qWarning() << "[Sprite] No pudo cargar:" << path;

        m_frames[state].append(pix);
    }
}

/* ---------- Frames espejados ---------- */
void Sprite::generateMirroredFrames(SpriteState srcState,
                                    SpriteState dstState)
{
    m_frames[dstState].clear();

    if (!m_frames.contains(srcState) || m_frames[srcState].isEmpty()) {
        qWarning() << "[Sprite] No hay frames en" << int(srcState)
        << "para espejar.";
        return;
    }
    for (const QPixmap& orig : m_frames[srcState]) {
        if (orig.isNull())
            m_frames[dstState].append(QPixmap());
        else
            m_frames[dstState].append(
                orig.transformed(QTransform().scale(-1, 1)));
    }
}

/* ---------- Transformaciones ---------- */
void Sprite::setPosition(int x, int y) { m_pos = {x, y}; }
void Sprite::setSize(int w, int h)     { m_drawSize = {w, h}; }

/* ---------- Cambiar estado ---------- */
void Sprite::setState(SpriteState newState)
{
    if (m_state != newState) {
        m_state           = newState;
        m_frameIndex      = 0;
        m_timeAccumulator = 0.0f;
    }
}

/* ---------- FPS ---------- */
void Sprite::setFPS(int fps)
{
    if (fps > 0) m_secondsPerFrame = 1.0f / float(fps);
}

/* ---------- Update por dt ---------- */
void Sprite::update(float dt)
{
    const QVector<QPixmap>& frames = m_frames.value(m_state);
    if (frames.isEmpty()) return;

    m_timeAccumulator += dt;
    while (m_timeAccumulator >= m_secondsPerFrame) {
        m_timeAccumulator -= m_secondsPerFrame;
        m_frameIndex = (m_frameIndex + 1) % frames.size();
    }
}

/* ---------- Draw ---------- */
void Sprite::draw(QPainter& painter) const
{
    const QVector<QPixmap>& frames = m_frames.value(m_state);
    if (frames.isEmpty()) return;

    const QPixmap& orig = frames.at(m_frameIndex);
    if (orig.isNull())   return;

    QPixmap scaled = orig.scaled(m_drawSize,
                                 Qt::KeepAspectRatio,
                                 Qt::SmoothTransformation);
    painter.drawPixmap(m_pos.x(), m_pos.y(), scaled);
}

/* ---------- currentFrame ---------- */
QPixmap Sprite::currentFrame() const
{
    const QVector<QPixmap>& vec = m_frames.value(m_state);
    return vec.isEmpty() ? QPixmap() : vec.at(m_frameIndex);
}
