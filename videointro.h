#ifndef VIDEOINTRO_H
#define VIDEOINTRO_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QVBoxLayout>

class VideoIntro : public QWidget
{
    Q_OBJECT

public:
    explicit VideoIntro(QWidget *parent = nullptr);

signals:
    void videoTerminado();

private:
    QMediaPlayer *player;
    QVideoWidget *videoWidget;
};

#endif // VIDEOINTRO_H
