#include "CombateManager.h"
#include "jugador.h"
#include "enemigo.h"
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
}

QRectF CombateManager::espadaRect(Jugador* j) const
{
    QGraphicsPixmapItem* item = j->graphicsItem();
    QRectF r = item->sceneBoundingRect();
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

    m_playerCd = qMax(0.0f, m_playerCd - dt);
    m_enemyCd  = qMax(0.0f, m_enemyCd  - dt);
    m_bossCd   = qMax(0.0f, m_bossCd   - dt);

    if (m_playerCd <= 0.0f) {
        auto st = m_player->sprite().getState();
        if (st == SpriteState::Slashing ||
            st == SpriteState::SlashingLeft)
        {
            QRectF hit = espadaRect(m_player);
            for (Enemigo* e : m_enemigos) {
                if (!e || e->isDead()) continue;
                if (hit.intersects(e->sceneBoundingRect()) && m_bossCd <= 0.0f) {
                    int baseDmg = m_player->damage();
                    int realDmg = m_player->computeDamage(baseDmg);
                    e->takeDamage(realDmg);

                    m_playerCd = COOLDOWN;
                    m_bossCd   = COOLDOWN;
                    break;
                }
            }
        }
    }

    if (m_enemyCd <= 0.0f) {
        QGraphicsPixmapItem* pItem = m_player->graphicsItem();
        if (!pItem) return;
        QRectF rectP = pItem->sceneBoundingRect();

        for (Enemigo* e : m_enemigos) {
            if (!e || e->isDead()) continue;
            if (e->estado() != Enemigo::Estado::Attack) continue;

            if (garraRect(e).intersects(rectP)) {
                int enemyDmg = e->damage();
                m_player->aplicarDano(enemyDmg);
                m_enemyCd = COOLDOWN;
                break;
            }
        }
    }
}

