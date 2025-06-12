#include "CombateManager.h"
#include "jugador.h"
#include "Enemigo.h"
#include "sprite.h"
#include <QtMath>

/* ---- Parámetros ---- */
static constexpr float COOLDOWN = 0.4f;
static constexpr int   DMG_P2E  = 1;   // daño jugador→enemigo
static constexpr int   DMG_E2P  = 1;   // daño enemigo→jugador
static constexpr int   REACH_P  = 25;  // alcance espada extra
static constexpr int   REACH_E  = 20;  // alcance garra extra

/* ---------- Helpers de hit-box ---------- */
QRectF CombateManager::espadaRect(Jugador* j) const
{
    QSize  spr = j->sprite().getSize();
    QPointF p  = j->transform().getPosition();
    int w = spr.width(), h = spr.height();

    bool left = (j->sprite().getState() == SpriteState::SlashingLeft);
    return left
               ? QRectF(p.x() - w/2 - REACH_P, p.y() - h, w/2 + REACH_P, h)
               : QRectF(p.x(),               p.y() - h, w/2 + REACH_P, h);
}

QRectF CombateManager::garraRect(Enemigo* e) const
{
    QSize  sz = e->pixmap().size();
    QPointF p = e->pos();
    int dir = (e->velX() >= 0) ? 1 : -1;
    if (e->estado() != Enemigo::Estado::Attack) dir = 1;

    return dir > 0
               ? QRectF(p.x(),                   p.y()-sz.height()/2,
                        sz.width()/2+REACH_E,    sz.height())
               : QRectF(p.x()-sz.width()/2-REACH_E, p.y()-sz.height()/2,
                        sz.width()/2+REACH_E,        sz.height());
}

/* ---------- Constructor ---------- */
CombateManager::CombateManager(Jugador* player,
                               QVector<Enemigo*>& enemigos,
                               QObject* parent)
    : QObject(parent), m_player(player), m_enemigos(enemigos)
{}

/* ---------- update ---------- */
void CombateManager::update(float dt)
{
    if (!m_player) return;

    m_playerCd = qMax(0.0f, m_playerCd - dt);
    m_enemyCd  = qMax(0.0f, m_enemyCd  - dt);

    /* ===== Ataque del Jugador ===== */
    SpriteState st = m_player->sprite().getState();
    bool attacking = (st == SpriteState::Slashing ||
                      st == SpriteState::SlashingLeft);

    if (attacking && m_playerCd <= 0.0f) {
        QRectF hit = espadaRect(m_player);
        for (Enemigo* e : m_enemigos) {
            if (!e || e->isDead()) continue;

            QSize sz = e->pixmap().size();
            QRectF rectE(e->pos().x() - sz.width()/2.0,
                         e->pos().y() - sz.height()/2.0,
                         sz.width(), sz.height());

            if (hit.intersects(rectE)) {
                e->takeDamage(DMG_P2E);
                m_playerCd = COOLDOWN;
            }
        }
    }

    /* ===== Ataque del Enemigo ===== */
    if (m_enemyCd <= 0.0f) {
        QSize ps  = m_player->sprite().getSize();
        QPointF pp = m_player->transform().getPosition();
        QRectF rectP(pp.x() - ps.width()/2.0,
                     pp.y() - ps.height(),
                     ps.width(), ps.height());

        for (Enemigo* e : m_enemigos) {
            if (!e || e->isDead()) continue;
            if (e->estado() != Enemigo::Estado::Attack) continue;

            int idx = e->frameIndex();          // frame actual
            if (idx < 1 || idx > 2) continue;   // golpe frames 1-2

            if (garraRect(e).intersects(rectP)) {
                m_player->aplicarDano(DMG_E2P);
                m_enemyCd = COOLDOWN;
            }
        }
    }
}
