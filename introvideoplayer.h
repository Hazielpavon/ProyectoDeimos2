#pragma once

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QVBoxLayout>
#include <functional>

class IntroVideoPlayer : public QWidget {
    Q_OBJECT

public:
    IntroVideoPlayer(const QString& videoPath, std::function<void()> onFinish, QWidget* parent = nullptr);

private:
    QMediaPlayer* m_player;
    QVideoWidget* m_videoWidget;
    std::function<void()> m_onFinish;
};
