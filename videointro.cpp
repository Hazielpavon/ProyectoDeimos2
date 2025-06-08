#include "videointro.h"
#include <QAudioOutput>

VideoIntro::VideoIntro(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(950, 650);

    videoWidget = new QVideoWidget(this);
    videoWidget->setFixedSize(this->size());

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.8);

    player->setVideoOutput(videoWidget);

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            emit videoTerminado();
            this->close();
        }
    });
}

void VideoIntro::setVideo(const QString &ruta) {
    player->setSource(QUrl(ruta));
    player->play();
}
