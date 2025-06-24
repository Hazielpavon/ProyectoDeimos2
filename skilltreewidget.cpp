/* ============================================================
 *  SkillTreeWidget.cpp   –   Árbol binario de habilidades
 *      • Iconos en botones
 *      • Fondo translúcido + líneas de conexión
 *      • Efectos operativos:  Super-Salto I / II,
 *                             Daño I / II,
 *                             Regeneración I
 * ============================================================ */
#include "SkillTreeWidget.h"
#include "jugador.h"

#include <QShowEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QGridLayout>
#include <QIcon>

/* ---------- icono para cada nodo ------------------------------------ */
static const char* ICON_PATHS[SkillTreeWidget::NODE_COUNT] = {
    ":/resources/jump1.png",        // 0  Super-Salto I
    ":/resources/damage1.png",      // 1  Daño I
    ":/resources/resistance1.png",  // 2  (libre)
    ":/resources/jump2.png",        // 3  Super-Salto II
    ":/resources/damage2.png",      // 4  Daño II
    ":/resources/resistance2.png",  // 5  (libre)
    ":/resources/regen.png"         // 6  Regeneración I
};

/* ============================================================
 *  ctor
 * ============================================================ */
SkillTreeWidget::SkillTreeWidget(Jugador* player, QWidget* parent)
    : QWidget(parent)
    , m_player(player)
{
    /* apariencia general ------------------------------------ */
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(460, 400);

    /* botones ------------------------------------------------ */
    const QSize BTN_SZ(64, 64);
    for (int i = 0; i < NODE_COUNT; ++i) {
        m_nodes[i] = new QPushButton(this);
        m_nodes[i]->setIcon(QIcon(ICON_PATHS[i]));
        m_nodes[i]->setIconSize(BTN_SZ);
        m_nodes[i]->setFixedSize(BTN_SZ);
        m_nodes[i]->setFlat(true);
        m_nodes[i]->setEnabled(i == 0);                 // solo raíz activa
        connect(m_nodes[i], &QPushButton::clicked,
                this, &SkillTreeWidget::onNodeClicked);
    }

    /* etiqueta “Puntos” ------------------------------------- */
    m_pointsLbl = new QLabel(this);
    m_pointsLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_pointsLbl->setStyleSheet("font: bold 14px; color:white;");

    m_points = 10;                                       // demo inicial
    initLayout();
    refreshPointsLabel();
}

/* ============================================================
 *  Construcción de la cuadrícula
 * ============================================================ */
void SkillTreeWidget::initLayout()
{
    auto* grid = new QGridLayout;
    grid->setHorizontalSpacing(28);
    grid->setVerticalSpacing(36);
    grid->setContentsMargins(28, 28, 28, 28);

    /* árbol binario 0-6   (último nodo: regen) */
    grid->addWidget(m_nodes[0], 0, 2);   // 0
    grid->addWidget(m_nodes[1], 1, 1);   // 1
    grid->addWidget(m_nodes[2], 1, 3);   // 2
    grid->addWidget(m_nodes[3], 2, 0);   // 3
    grid->addWidget(m_nodes[4], 2, 2);   // 4
    grid->addWidget(m_nodes[5], 2, 4);   // 5
    grid->addWidget(m_nodes[6], 3, 2);   // 6 (Regen I)

    /* puntos arriba-derecha */
    grid->addWidget(m_pointsLbl, 0, 4, Qt::AlignRight);

    setLayout(grid);          // guardado en this->layout()
}

/* ============================================================
 *  Centrarse sobre la ventana padre
 * ============================================================ */
void SkillTreeWidget::showEvent(QShowEvent* ev)
{
    QWidget::showEvent(ev);
    if (auto* p = parentWidget()) {
        move(p->mapToGlobal(
            QPoint((p->width()  - width())  / 2,
                   (p->height() - height()) / 2)));
    }
}

/* ============================================================
 *  Fondo y líneas de conexión
 * ============================================================ */
void SkillTreeWidget::paintEvent(QPaintEvent* ev)
{
    Q_UNUSED(ev);
    QPainter g(this);
    g.setRenderHint(QPainter::Antialiasing, true);

    /* fondo translúcido */
    g.setBrush(QColor(20, 20, 30, 240));
    g.setPen(Qt::NoPen);
    g.drawRoundedRect(rect().adjusted(2, 2, -2, -2), 16, 16);

    /* conexiones */
    g.setPen(QPen(QColor(210, 210, 210, 180), 2, Qt::SolidLine, Qt::RoundCap));

    struct Pos { int row, col; };
    const Pos PAIRS[][2] = {
        {{0,2}, {1,1}}, {{0,2}, {1,3}},   // raíz-hijos
        {{1,1}, {2,0}}, {{1,1}, {2,2}},   // rama izq
        {{1,3}, {2,2}}, {{1,3}, {2,4}},   // rama der
        {{2,2}, {3,2}}                    // hasta regen
    };

    auto* grid = static_cast<QGridLayout*>(layout());
    for (auto& pr : PAIRS) {
        QWidget* a = grid->itemAtPosition(pr[0].row, pr[0].col)->widget();
        QWidget* b = grid->itemAtPosition(pr[1].row, pr[1].col)->widget();
        if (a && b)
            g.drawLine(a->geometry().center(), b->geometry().center());
    }
}

/* ============================================================
 *  Puntos
 * ============================================================ */
void SkillTreeWidget::addPoints(int n)
{
    m_points += n;
    refreshPointsLabel();
}

/* ============================================================
 *  Click en un nodo
 * ============================================================ */
void SkillTreeWidget::onNodeClicked()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int idx = -1;
    for (int i = 0; i < NODE_COUNT && idx == -1; ++i)
        if (btn == m_nodes[i]) idx = i;

    if (idx < 0 || m_unlocked.contains(idx) || m_points <= 0) return;

    /* compra */
    --m_points;
    m_unlocked.insert(idx);
    btn->setStyleSheet("border:2px solid #66c37a; border-radius:6px;");
    refreshPointsLabel();
    unlockChildren(idx);
    applyEffect(idx);
}

/* ============================================================
 *  Habilitar hijos
 * ============================================================ */
void SkillTreeWidget::unlockChildren(int idx)
{
    int l = idx * 2 + 1;
    int r = idx * 2 + 2;
    if (l < NODE_COUNT) m_nodes[l]->setEnabled(true);
    if (r < NODE_COUNT) m_nodes[r]->setEnabled(true);
}

/* ============================================================
 *  “Puntos : n”
 * ============================================================ */
void SkillTreeWidget::refreshPointsLabel()
{
    m_pointsLbl->setText(QStringLiteral("Puntos: %1").arg(m_points));
}

/* ============================================================
 *  Aplicar efecto al jugador
 * ============================================================ */
void SkillTreeWidget::applyEffect(int idx)
{
    if (!m_player) return;

    switch (idx) {
    case 0:  // Super-Salto I
        m_player->setJumpMultiplier(2.0f);
        emit superJump1Unlocked();
        break;
    case 3:  // Super-Salto II
        m_player->setJumpMultiplier(2.5f);
        break;

    case 1:  // Daño I
        m_player->setDamageMultiplier(1.5f);
        break;
    case 4:  // Daño II
        m_player->setDamageMultiplier(2.0f);
        break;

    case 6: { // Regen I  (+25 % vida)
        int extra = m_player->maxHP() / 4;
        m_player->setHP(qMin(m_player->maxHP(),
                             m_player->currentHP() + extra));
    } break;

    default:
        break;      // nodos 2 / 5 sin efecto aún
    }
}
