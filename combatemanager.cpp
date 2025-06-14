#include "CombateManager.h"
#include "jugador.h"
#include "Enemigo.h"
#include <QGraphicsPixmapItem>
#include <QtMath>
#include <QDebug>

static constexpr float COOLDOWN = 0.4f;
static constexpr int   DMG_P2E  = 1;
static constexpr int   DMG_E2P  = 1;
static constexpr int   REACH_P  = 25;
static constexpr int   REACH_E  = 20;

CombateManager::CombateManager(Jugador* player,
                               QVector<Enemigo*>& enemigos,
                               QObject* parent)
    : QObject(parent)
    , m_player(player)
    , m_enemigos(enemigos)
    , m_playerCd(0.0f)
    , m_enemyCd(0.0f)
{
    // nada más
}

QRectF CombateManager::espadaRect(Jugador* j) const
{
    // Partimos del rect real en escena
    // (necesitamos exponer en Jugador un getter al QGraphicsPixmapItem*)
    QGraphicsPixmapItem* item = j->graphicsItem();
    QRectF r = item->sceneBoundingRect();

    // expandimos hacia donde esté slashing
    auto st = j->sprite().getState();
    if (st == SpriteState::SlashingLeft)
        r.adjust(-REACH_P, 0, 0, 0);
    else
        r.adjust(0, 0, +REACH_P, 0);

    return r;
}

QRectF CombateManager::garraRect(Enemigo* e) const
{
    QRectF r = e->sceneBoundingRect();

    // sólo añadimos reach en el frame de ataque
    if (e->estado() == Enemigo::Estado::Attack) {
        int dir = (e->velX() >= 0 ? +1 : -1);
        if (dir > 0)  r.adjust(0, 0, +REACH_E, 0);
        else          r.adjust(-REACH_E, 0, 0,  0);
    }
    return r;
}

void CombateManager::update(float dt)
{
    if (!m_player) return;

    // enfriamientos
    m_playerCd = qMax(0.0f, m_playerCd - dt);
    m_enemyCd  = qMax(0.0f, m_enemyCd  - dt);
    m_bossCd  = qMax(0.0f, m_bossCd  - dt);

    // 1) ATAQUE DEL JUGADOR
    if (m_playerCd <= 0.0f) {
        auto st = m_player->sprite().getState();
        if (st == SpriteState::Slashing ||
            st == SpriteState::SlashingLeft)
        {
            QRectF hit = espadaRect(m_player);
            int n = m_enemigos.size();
            for (int i = 0; i < n; ++i) {
                Enemigo* e = m_enemigos[i];
                if (!e || e->isDead()) continue;
                QRectF rectE = e->sceneBoundingRect();
                if (hit.intersects(rectE) && m_bossCd <= 0.0f) {
                    e->takeDamage(DMG_P2E);
                    m_playerCd = COOLDOWN;
                    m_bossCd   = COOLDOWN;
                    break;
                }
            }
        }
    }

    // 2) ATAQUE DEL ENEMIGO
    if (m_enemyCd <= 0.0f) {
        // 1) Obtén el pixmap del jugador y comprueba nullptr
        QGraphicsPixmapItem* pItem = m_player->graphicsItem();
        if (!pItem) {
            qWarning() << "[CombateManager] graphicsItem del jugador no set!";
            return;
        }
        QRectF rectP = pItem->sceneBoundingRect();

        // 2) Ahora el bucle por índices
        int n = m_enemigos.size();
        for (int i = 0; i < n; ++i) {
            Enemigo* e = m_enemigos[i];
            if (!e || e->isDead()) continue;
            if (e->estado() != Enemigo::Estado::Attack) continue;

            int frame = e->frameIndex();

            if (garraRect(e).intersects(rectP)) {
                m_player->aplicarDano(DMG_E2P);
                m_enemyCd = COOLDOWN;
                break;
            }
        }
    }
}
