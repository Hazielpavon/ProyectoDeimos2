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

    QString texto;

    if (!m_hasQuest) {
        texto = "¿Podrías matar al Minotauro que acecha esta torre?";
        m_hasQuest = true;
        m_accepted = true;
    }
    else if (m_hasQuest && !m_accepted) {
        texto = "¿Seguro que no cambias de opinión?";
    }
    else if (m_hasQuest && m_accepted && !m_rewardGiven) {
        if (m_level->isBossDefeated()) {
            texto = "¡Gracias por tu valentía! Aquí tienes tu recompensa.";
            m_level->rewardPlayerExtraDamage();
            m_rewardGiven = true;
        } else {
            texto = "¿Ya lo mataste? ¡Apresúrate!";
        }
    }

    // Mostrar texto y fondo
    m_dialogoTexto->setPlainText(texto);
    QRectF textRect = m_dialogoTexto->boundingRect();
    QPointF npcPos = sceneBoundingRect().center();

    // Alinear el texto y el fondo encima del NPC
    QPointF textoPos(npcPos.x() - textRect.width() / 2, npcPos.y() - 120);
    m_dialogoTexto->setPos(textoPos);

    QRectF fondoRect = textRect.adjusted(-10, -10, 10, 10);
    m_dialogoFondo->setRect(fondoRect);
    m_dialogoFondo->setPos(textoPos);

    m_dialogoTexto->setVisible(true);
    m_dialogoFondo->setVisible(true);
    m_timerOcultarTexto->start(3000);  // ocultar en 3 segundos

    m_state = Idle;
    m_frameIndex = 0;
    m_animTimer = 0;
}
void NPC::onBossDefeated()
{
    // si aceptaste la misión y aún no diste la recompensa, forzamos el diálogo:
    if (m_hasQuest && m_accepted && !m_rewardGiven) {
        // para ignorar cualquier cooldown pendiente:
        m_talkCooldown = 0.0f;
        talk();
    }

}
