#include "npc_tutorial.h"
#include "tutorialscene.h"
#include "jugador.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QtMath>

npc_tutorial::npc_tutorial(TutorialScene* level,
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

    // Posición fija del sprite
    setPixmap(m_idleFrames[0]);
    setOffset(-pixmap().width() / 2, -pixmap().height());
    setPos(pos);
    setZValue(4);
    scene->addItem(this);

    // Crear el texto sobre la cabeza del NPC
    m_textItem = new QGraphicsTextItem();
    m_textItem->setDefaultTextColor(QColor(220, 220, 255));  // Azul claro estilo Hollow Knight
    m_textItem->setFont(QFont("Georgia", 14, QFont::Bold));
    m_textItem->setZValue(5);
    m_textItem->setPlainText("");  // Se muestra luego
    m_textItem->setTextWidth(200); // Para multilínea automática
    m_textItem->setPos(pos.x() - 100, pos.y() - pixmap().height() - 40);
    scene->addItem(m_textItem);
}

void npc_tutorial::loadAnimations()
{
    m_idleFrames.push_back(QPixmap(":/resources/Npc1Idle1.png"));
    m_idleFrames.push_back(QPixmap(":/resources/Npc1Idle2.png"));
    m_idleFrames.push_back(QPixmap(":/resources/Npc1Idle3.png"));
    m_idleFrames.push_back(QPixmap(":/resources/Npc1Idle4.png"));

    m_speakFrames.push_back(QPixmap(":/resources/Npc1Speaking1.png"));
    m_speakFrames.push_back(QPixmap(":/resources/Npc1Speaking2.png"));
    m_speakFrames.push_back(QPixmap(":/resources/Npc1Speaking3.png"));
    m_speakFrames.push_back(QPixmap(":/resources/Npc1Speaking4.png"));
}

void npc_tutorial::update(float dt)
{
    // 1) animación
    m_animTimer += dt;
    auto& frames = (m_state == Idle ? m_idleFrames : m_speakFrames);
    if (m_animTimer > 0.2f) {
        m_animTimer = 0;
        m_frameIndex = (m_frameIndex + 1) % frames.size();
        setPixmap(frames[m_frameIndex]);
    }

    // 2) cooldown
    m_talkCooldown -= dt;

    // 3) proximidad + diálogo
    QPointF npcCenter = sceneBoundingRect().center();
    QPointF playerCenter = m_player->graphicsItem()->sceneBoundingRect().center();
    float d = std::hypot(npcCenter.x() - playerCenter.x(), npcCenter.y() - playerCenter.y());

    if (d < m_proximity && m_talkCooldown <= 0.0f) {
        talk();
        m_talkCooldown = 3.0f;  // espera antes de volver a hablar
    }
}

void npc_tutorial::talk()
{
    if (m_state == Talking) return;
    m_state = Talking;

    if (m_textItem) {
        m_textItem->setPlainText("Bienvenido, Fobos...\nPrepárate,para la oscuridad.");
    }

    QTimer::singleShot(6000, this, [this]() {
        if (m_textItem) m_textItem->setPlainText("");
    });

    m_state = Idle;
    m_frameIndex = 0;
    m_animTimer = 0;
}

void npc_tutorial::onBossDefeated()
{
    // No se usa en esta versión, pero puedes mostrar un mensaje especial si quieres.
}
