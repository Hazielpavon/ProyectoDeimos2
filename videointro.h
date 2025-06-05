#ifndef VIDEOINTRO_H
#define VIDEOINTRO_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>

class VideoIntro : public QWidget
{
    Q_OBJECT

public:
    explicit VideoIntro(QWidget *parent = nullptr);
    void setVideo(const QString &ruta);

signals:
    void videoTerminado();

private:
    QMediaPlayer *player;
    QVideoWidget *videoWidget;
    QAudioOutput *audioOutput;
};

#endif // VIDEOINTRO_H
