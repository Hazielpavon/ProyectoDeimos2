#include "npc.h"
#include "niveltorredelamarca.h"
#include "jugador.h"
#include <QGraphicsScene>
#include <QMessageBox>
#include <QtMath>

NPC::NPC(niveltorredelamarca* level,
         Jugador* player,
         QGraphicsScene* scene,
         const QPointF& pos,
         QObject* parent)
    : QObject(parent)
    , QGraphicsPixmapItem()
    , m_player(player)
    , m_level(level)
{
    loadAnimations();
    // posición fija:
    setPixmap(m_idleFrames[0]);
    setOffset(-pixmap().width()/2, -pixmap().height());
    setPos(pos);
    setZValue(4);
    scene->addItem(this);
}

void NPC::loadAnimations()
{
    // cargar fotogramas (asume que ya has importado tus 4 imágenes)
    m_idleFrames.push_back(QPixmap(":/resources/Npc1Idle1.png"));
    m_idleFrames.push_back(QPixmap(":/resources/Npc1Idle2.png"));
    m_idleFrames.push_back(QPixmap(":/resources/Npc1Idle3.png"));
    m_idleFrames.push_back(QPixmap(":/resources/Npc1Idle4.png"));

    m_speakFrames.push_back(QPixmap(":/resources/Npc1Speaking1.png"));
    m_speakFrames.push_back(QPixmap(":/resources/Npc1Speaking2.png"));
    m_speakFrames.push_back(QPixmap(":/resources/Npc1Speaking3.png"));
    m_speakFrames.push_back(QPixmap(":/resources/Npc1Speaking4.png"));
}

void NPC::update(float dt)
{
    // 1) animación
    m_animTimer += dt;
    auto &frames = (m_state==Idle ? m_idleFrames : m_speakFrames);
    if (m_animTimer > 0.2f) {
        m_animTimer = 0;
        m_frameIndex = (m_frameIndex+1) % frames.size();
        setPixmap(frames[m_frameIndex]);
    }

    // 2) cuenta atrás del cooldown de diálogo
    m_talkCooldown -= dt;

    // 3) proximidad + diálogo cada 3s
    QPointF npcCenter    = sceneBoundingRect().center();
    QPointF playerCenter = m_player->graphicsItem()
                               ->sceneBoundingRect()
                               .center();
    float d = std::hypot(npcCenter.x()-playerCenter.x(),
                         npcCenter.y()-playerCenter.y());

    if (d < m_proximity && m_talkCooldown <= 0.0f) {
        talk();
        m_talkCooldown = 3.0f;  // esperar 3s antes de volver a preguntar
    }
}


void NPC::talk()
{
    if (m_state == Talking) return;
    m_state = Talking;

    // 1) ¿Trajo la llave TRAS haber matado al boss?
    if (m_hasQuest && m_accepted && m_rewardGiven && !m_keyDelivered &&
        m_player->hasKey("Torre De La Marca"))
    {
        m_keyDelivered = true;
        m_player->useKey("Torre De La Marca");
        QMessageBox::information(
            nullptr, "El Sabio",
            "¡Excelente! Aquí tienes tu recompensa final por tu valentía.");
    }
    // 2) Ofrecer la misión si nunca la he ofrecido
    else if (!m_hasQuest) {
        auto r = QMessageBox::question(
            nullptr, "El Sabio",
            "¿Podrías matar al Minotauro que acecha esta torre? ¡Te recompensaré!",
            QMessageBox::Yes|QMessageBox::No);
        m_hasQuest = true;
        m_accepted = (r == QMessageBox::Yes);
        if (!m_accepted)
            m_level->penalizarCanones();
    }
    // 3) Ya aceptó, pero aún no mata al boss
    else if (m_hasQuest && m_accepted && !m_rewardGiven) {
        if (m_level->isBossDefeated()) {
            m_level->rewardPlayerExtraDamage();
            m_rewardGiven = true;
            QMessageBox::information(
                nullptr, "El Sabio",
                "¡Has vencido al Minotauro! Mi gratitud y un poder extra para ti.");
        } else {
            QMessageBox::information(
                nullptr, "El Sabio",
                "¿Ya lo mataste? ¡Apresúrate!");
        }
    }
    // 4) Ya mató al boss y recibió mejora, pero aún falta la llave
    else if (m_hasQuest && m_accepted && m_rewardGiven && !m_keyDelivered) {
        QMessageBox::information(
            nullptr, "El Sabio",
            "Bien hecho. Cuando traigas la llave que dejó el Minotauro, tendrás tu recompensa final.");
    }
    // 5) Ya entregada la llave: silencio o diálogo extra
    else {
        // Aquí podrías dejarle callado, o un mensaje breve:
        // QMessageBox::information(nullptr, "El Sabio", "Gracias de nuevo.");
    }

    m_state      = Idle;
    m_frameIndex = 0;
    m_animTimer  = 0;
}
void NPC::onBossDefeated()
{
    // Forzamos el diálogo apenas el boss caiga, si ya aceptó
    if (m_hasQuest && m_accepted && !m_rewardGiven) {
        m_talkCooldown = 0.0f;
        talk();
    }
}
