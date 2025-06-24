#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QSet>

class Jugador;


class SkillTreeWidget : public QWidget
{
    Q_OBJECT
public:
    static constexpr int NODE_COUNT = 7;

    explicit SkillTreeWidget(Jugador *player, QWidget *parent = nullptr);

    void addPoints(int n);
    int  points() const { return m_points; }

    void setPlayer(Jugador *p) { m_player = p; }

signals:
    void superJump1Unlocked();

protected:
    void showEvent (QShowEvent *ev) override;
    void paintEvent(QPaintEvent *ev) override;

private slots:
    void onNodeClicked();

private:
    void initLayout();
    void refreshPointsLabel();
    void unlockChildren(int idx);
    void applyEffect   (int idx);

    QPushButton* m_nodes[NODE_COUNT]{};
    QLabel      *m_pointsLbl   = nullptr;
    QGridLayout *m_grid        = nullptr;

    QSet<int>    m_unlocked;
    int          m_points   = 0;
    Jugador*     m_player   = nullptr;
};
