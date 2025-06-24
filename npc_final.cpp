#include "npc_final.h"
#include "jugador.h"
#include "maquina_olvido.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QLineF>

npc_final::npc_final(maquina_olvido* nivel,
                     Jugador* player,
                     QGraphicsScene* scene,
                     const QPointF& pos,
                     QObject* parent)
    : QObject(parent), QGraphicsPixmapItem(), m_level(nivel), m_player(player)
{
    loadAnimations();

    // Posición y sprite
    setPixmap(m_idleFrames[0]);
    setOffset(-pixmap().width() / 2, -pixmap().height());
    setPos(pos);
    setZValue(4);
    scene->addItem(this);

    // Textos que se mostrarán
    m_textos = {
        "Has llegado al final de tu camino.",
        "Pocos han sobrevivido tanto como tú.",
        "El mundo ha cambiado… y tú con él.",
        "La oscuridad que enfrentaste no era externa… sino interna.",
        "Gracias por jugar. ¿Estás listo para tu destino?"
    };

    // Texto flotante (sin fondo)
    m_dialogoTexto = new QGraphicsTextItem();
    m_dialogoTexto->setDefaultTextColor(Qt::white);
    m_dialogoTexto->setFont(QFont("Arial", 14));
    m_dialogoTexto->setTextWidth(580);
    m_dialogoTexto->setZValue(10);
    m_dialogoTexto->setPos(190, 480);  // Posición fija
    m_dialogoTexto->hide();
    scene->addItem(m_dialogoTexto);

    // Timer para animar sprite
    m_animTimer = new QTimer(this);
    connect(m_animTimer, &QTimer::timeout, this, &npc_final::updateAnimacion);
    m_animTimer->start(200);

    // Timer para cambiar texto automáticamente
    m_textoTimer = new QTimer(this);
    connect(m_textoTimer, &QTimer::timeout, this, &npc_final::siguienteTexto);

    // Timer para detectar cercanía con el jugador
    QTimer* timerProximidad = new QTimer(this);
    connect(timerProximidad, &QTimer::timeout, this, &npc_final::verificarProximidad);
    timerProximidad->start(300);  // cada 0.3 segundos

    m_mostrando = false;
    m_indiceTexto = 0;

    // Enlazar con el nivel (si se requiere)
    if (m_level)
        m_level->setNPCFinal(this);
}

void npc_final::loadAnimations()
{
    m_idleFrames = {
        QPixmap(":/resources/Npc1Idle1.png"),
        QPixmap(":/resources/Npc1Idle2.png"),
        QPixmap(":/resources/Npc1Idle3.png"),
        QPixmap(":/resources/Npc1Idle4.png")
    };

    m_speakFrames = {
        QPixmap(":/resources/Npc1Speaking1.png"),
        QPixmap(":/resources/Npc1Speaking2.png"),
        QPixmap(":/resources/Npc1Speaking3.png"),
        QPixmap(":/resources/Npc1Speaking4.png")
    };
}

void npc_final::update(float dt)
{
    Q_UNUSED(dt);
}

void npc_final::updateAnimacion()
{
    const auto& frames = m_mostrando ? m_speakFrames : m_idleFrames;
    if (!frames.isEmpty()) {
        m_frameIndex = (m_frameIndex + 1) % frames.size();
        setPixmap(frames[m_frameIndex]);
    }
}

void npc_final::mostrarTextoActual()
{
    if (m_indiceTexto < m_textos.size()) {
        m_dialogoTexto->setPlainText(m_textos[m_indiceTexto]);
        m_dialogoTexto->show();
    } else {
        m_mostrando = false;
        m_textoTimer->stop();
        m_dialogoTexto->hide();
    }
}

void npc_final::siguienteTexto()
{
    if (m_mostrando) {
        m_indiceTexto++;
        mostrarTextoActual();
    }
}

void npc_final::verificarProximidad()
{
    if (!m_player || !m_player->graphicsItem()) return;

    QPointF jugadorPos = m_player->graphicsItem()->pos();
    QPointF npcPos = this->pos();

    qreal distancia = QLineF(jugadorPos, npcPos).length();

    if (distancia < 150.0) {
        if (!m_mostrando) {
            m_mostrando = true;
            m_indiceTexto = 0;
            mostrarTextoActual();
            m_textoTimer->start(5000);  // cambiar cada 5 segundos
        }
    } else {
        m_mostrando = false;
        m_textoTimer->stop();
        m_dialogoTexto->hide();
    }
}
