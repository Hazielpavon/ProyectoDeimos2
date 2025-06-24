#include "IntroVideoPlayer.h"
#include <QUrl>
#include <QFileInfo>

IntroVideoPlayer::IntroVideoPlayer(const QString& videoPath, std::function<void()> onFinish, QWidget* parent)
    : QWidget(parent), m_onFinish(onFinish)
{
    m_player = new QMediaPlayer(this);
    m_videoWidget = new QVideoWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_videoWidget);
    setLayout(layout);

    m_player->setVideoOutput(m_videoWidget);

    QUrl videoUrl = QUrl::fromLocalFile(QFileInfo(videoPath).absoluteFilePath());
    m_player->setSource(videoUrl);

    m_videoWidget->show();
    m_player->play();
    show();

    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::EndOfMedia) {
            m_player->stop();
            this->close();
            if (m_onFinish) m_onFinish();
        }
    });

    setFixedSize(950, 650);
}
