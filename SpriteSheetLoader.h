#pragma once
#include "enemigo.h"
#include <QPixmap>
#include <QVector>
#include <QDebug>

inline Animacion loadSheetRow(const QString& sheetPath,
                              int frames,
                              qreal scale = 1.0,
                              float fps   = 8.0f)
{
    Animacion a;  a.fps = fps;

    QPixmap sheet(sheetPath);
    if (sheet.isNull()) {
        qWarning() << "[loadSheetRow] No se cargó:" << sheetPath;
        return a;
    }

    int frameW = sheet.width()  / frames;
    int frameH = sheet.height();

    for (int i = 0; i < frames; ++i) {
        QPixmap frame = sheet.copy(i*frameW, 0, frameW, frameH);
        if (scale != 1.0)
            frame = frame.scaled(frame.size()*scale,
                                 Qt::KeepAspectRatio,
                                 Qt::SmoothTransformation);
        a.frames.append(frame);
    }
    return a;
}
