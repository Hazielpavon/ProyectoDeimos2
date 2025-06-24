#ifndef NPC_FINAL_H
#define NPC_FINAL_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QVector>
#include <QString>

class Jugador;
class maquina_olvido;
class QGraphicsScene;
class QGraphicsTextItem;

class npc_final : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    npc_final(maquina_olvido* nivel,
              Jugador* player,
              QGraphicsScene* scene,
              const QPointF& pos,
              QObject* parent = nullptr);

    void update(float dt);
    void siguienteTexto();

private slots:
    void updateAnimacion();
    void mostrarTextoActual();
    void verificarProximidad();  // ← AÑADIDO

private:
    void loadAnimations();

    QVector<QPixmap> m_idleFrames;
    QVector<QPixmap> m_speakFrames;
    int m_frameIndex = 0;

    QVector<QString> m_textos;
    int m_indiceTexto = 0;
    bool m_mostrando = false;

    QGraphicsTextItem* m_dialogoTexto = nullptr;

    QTimer* m_animTimer = nullptr;   // ← AÑADIDO
    QTimer* m_textoTimer = nullptr;  // ← AÑADIDO

    maquina_olvido* m_level = nullptr;
    Jugador* m_player = nullptr;
};

#endif // NPC_FINAL_H
