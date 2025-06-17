// ===========================================================
//  Skeleton.cpp  – versión final
//  • Cada hoja es de una sola fila
//  • Ajuste vertical fijo: pies = (0,0)
//    offset-y = −frameHeight + DY_FIX
// ===========================================================

#include "Skeleton.h"
#include "SpriteSheetLoader.h"
#include "jugador.h"

#include <QTransform>
#include <QGraphicsScene>

/* ----------------  Constantes generales  ---------------- */
namespace {
constexpr qreal  SCALE      = 1.6;   // agrandar sprites
constexpr int    GAP_PX     = 30;    // margen vacío en el PNG original
constexpr int    DY_FIX     = int(GAP_PX * SCALE + 0.5);   // ≃48px

constexpr float  ATK_RANGE  = 45.f;
constexpr float  DET_RANGE  = 260.f;
constexpr float  Y_TOL      = 10.f;
constexpr float  GRAV       = 600.f;
}

/* =========================================================
 *  Constructor
 * ========================================================= */
Skeleton::Skeleton(QObject* parent)
    : Enemigo(parent)
{
    addAnim(Estado::Idle  , loadSheetRow(":/resources/skeleton_idle.png"     , 4 , SCALE));
    addAnim(Estado::Walk  , loadSheetRow(":/resources/skeleton_walk.png"     , 4 , SCALE));
    addAnim(Estado::Attack, loadSheetRow(":/resources/skeleton_cleave.png"   , 8 , SCALE, 10.f));
    addAnim(Estado::Hurt  , loadSheetRow(":/resources/skeleton_take_hit.png" , 4 , SCALE));
    addAnim(Estado::Death , loadSheetRow(":/resources/skeleton_death.png"    , 4 , SCALE, 12.f));

    if (!animActual().frames.isEmpty()) {
        const QPixmap& f = animActual().frames.first();

        /* origen (0,0) = centro-X y pies */
        setOffset(-f.width() / 2, -f.height() + DY_FIX);
        setTransformOriginPoint(f.width() / 2, f.height() - DY_FIX);
    }
}

/* =========================================================
 *  Geometría para colisiones
 * ========================================================= */
QRectF Skeleton::boundingRect() const
{
    const QPixmap& p = pixmap();
    return { -p.width() / 2.0,          // izquierda
            -p.height() + DY_FIX,      // top  (pies = 0)
            p.width(), p.height() };  // ancho / alto
}

QPainterPath Skeleton::shape() const
{
    QPainterPath s;  s.addRect(boundingRect());  return s;
}

/* =========================================================
 *  IA (patrulla / persecución / ataque)
 * ========================================================= */
void Skeleton::updateAI(float dt)
{
    if (!target()) return;

    float dx = target()->transform().getPosition().x() - pos().x();

    float selfFeet = sceneBoundingRect().bottom();
    float targFeet = selfFeet;
    if (auto* j = dynamic_cast<Jugador*>(target()))
        if (auto* gi = j->graphicsItem())
            targFeet = gi->sceneBoundingRect().bottom();

    /* ataque */
    if (qAbs(dx) < ATK_RANGE && qAbs(targFeet - selfFeet) < Y_TOL) {
        setEstado(Estado::Attack);  m_velX = 0;
        m_faceRight = dx >= 0;      return;
    }

    /* persecución */
    if (qAbs(dx) < DET_RANGE) {
        setEstado(Estado::Walk);
        m_velX = (dx > 0 ? 1 : -1) * 100.f;
        m_faceRight = dx >= 0;      return;
    }

    /* patrulla */
    setEstado(Estado::Walk);
    m_patrolTime += dt;
    if (m_patrolTime > 2.5f) { m_patrolDir = -m_patrolDir; m_patrolTime = 0; }
    m_velX = m_patrolDir * 70.f;
    m_faceRight = m_patrolDir > 0;
}

/* =========================================================
 *  Daño / muerte
 * ========================================================= */
void Skeleton::takeDamage(int dmg)
{
    if (isDead()) return;
    Enemigo::takeDamage(dmg);

    if (isDead()) {
        m_dying = true;
        m_dieTimer = 0.f;
        setEstado(Estado::Death);
        auto& a = animActual();  a.idx = 0;  a.acum = 0;
    }
}

/* =========================================================
 *  Update general (60 FPS)
 * ========================================================= */
void Skeleton::update(float dt)
{
    /* IA + física mientras vive */
    if (!m_dying) {
        updateAI(dt);
        m_velY += GRAV * dt;
        moveBy(m_velX * dt, m_velY * dt);
    }

    /* Animación */
    auto& a = animActual();
    a.avanzar(dt);

    QPixmap frame = a.actual();
    if (m_faceRight)
        frame = frame.transformed(QTransform().scale(-1,1));

    /* Dibujar */
    prepareGeometryChange();
    setPixmap(frame);
    setOffset(-frame.width()/2, -frame.height() + DY_FIX);

    /* Fin de muerte */
    if (m_dying) {
        m_dieTimer += dt;
        if (m_dieTimer > 1.f / a.fps && a.idx == a.frames.size() - 1)
            setVisible(false);           // escena lo eliminará más tarde
    }
}
