#include "videointro.h"
#include <QAudioOutput>
#include <QFileInfo>
#include <QKeyEvent>
#include <QDebug>

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

    // Termina automáticamente cuando llega al final
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            emit videoTerminado();
            this->close();
        }
    });

    // Manejo de errores (muy importante)
    connect(player, &QMediaPlayer::errorOccurred, this, [](QMediaPlayer::Error err, const QString &errorString) {
        qDebug() << "[ERROR de video]" << err << errorString;
    });
}

void VideoIntro::setVideo(const QString &ruta)
{
    QString rutaAbsoluta = QFileInfo(ruta).absoluteFilePath();
    qDebug() << "[VideoIntro] Cargando:" << rutaAbsoluta;

    player->setSource(QUrl::fromLocalFile(rutaAbsoluta));
    player->play();
}

// Permitir saltar con Esc
void VideoIntro::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Return) {
        qDebug() << "[VideoIntro] Video saltado con tecla.";
        player->stop();
        emit videoTerminado();
        this->close();
    }
    QWidget::keyPressEvent(event);
}
