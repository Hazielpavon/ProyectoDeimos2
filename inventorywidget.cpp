#include "InventoryWidget.h"

#include <QGridLayout>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QApplication>
#include <QCursor>
#include <QFrame>
#include <QDebug>

/* ════════════════════════════════════════════════════════ *
 *                       ItemSlot                           *
 * ════════════════════════════════════════════════════════ */
ItemSlot::ItemSlot(QWidget* parent)
    : QLabel(parent)
{
    setFixedSize(48, 48);
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setAlignment(Qt::AlignCenter);
    setAcceptDrops(true);
}

/* coloca un ítem con icono y su id interno */
void ItemSlot::setItem(const QPixmap& pix, const QString& id)
{
    setPixmap(pix.scaled(size(), Qt::KeepAspectRatio,
                         Qt::SmoothTransformation));
    m_empty = false;
    m_id    = id;
}

void ItemSlot::clearSlot()
{
    clear();
    m_empty = true;
    m_id.clear();
}

/* iniciar arrastre si hay objeto */
void ItemSlot::mousePressEvent(QMouseEvent* ev)
{
    if (m_empty || ev->button() != Qt::LeftButton)
        return;

    /* empaquetamos id como texto */
    QMimeData* md = new QMimeData;
    md->setText(m_id);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(md);

#if QT_VERSION_MAJOR >= 6
    QPixmap pm = pixmap(Qt::ReturnByValue);       // Qt 6 devuelve por valor
#else
    QPixmap pm = *pixmap();                       // Qt 5 devolvía puntero
#endif
    drag->setPixmap(pm);
    drag->setHotSpot(QPoint(pm.width() / 2, pm.height() / 2));

    drag->exec(Qt::MoveAction);
    emit beginDrag(this);
}

void ItemSlot::mouseDoubleClickEvent(QMouseEvent*)
{
    if (!m_empty) clearSlot();
}

/* ════════════════════════════════════════════════════════ *
 *                    InventoryWidget                       *
 *  (sin cambios respecto a la versión anterior)            *
 * ════════════════════════════════════════════════════════ */
InventoryWidget* InventoryWidget::s_inst = nullptr;

InventoryWidget* InventoryWidget::instance(QWidget* parent)
{
    if (!s_inst) s_inst = new InventoryWidget(parent);
    return s_inst;
}

InventoryWidget::InventoryWidget(QWidget* parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
    setWindowTitle("Inventario");
    setFixedSize(300, 340);

    m_grid = new QGridLayout(this);
    m_grid->setSpacing(2);
    m_grid->setContentsMargins(6, 6, 6, 6);

    /* 25 ranuras 5×5 */
    for (int r = 0; r < 5; ++r)
        for (int c = 0; c < 5; ++c) {
            auto* slot = new ItemSlot(this);
            connect(slot, &ItemSlot::beginDrag,
                    this, &InventoryWidget::handleBeginDrag);
            m_grid->addWidget(slot, r, c);
        }

    /* basurero */
    m_trash = new ItemSlot(this);
    m_trash->setText("🗑");
    m_trash->setStyleSheet("font: 24px");
    m_trash->setAcceptDrops(true);
    m_grid->addWidget(m_trash, 5, 0, 1, 5, Qt::AlignCenter);
}

ItemSlot* InventoryWidget::firstFreeSlot() const
{
    for (int i = 0; i < m_grid->count(); ++i) {
        auto* slot = qobject_cast<ItemSlot*>(m_grid->itemAt(i)->widget());
        if (slot && slot->isEmpty())
            return slot;
    }
    return nullptr;
}

bool InventoryWidget::addKey(const QString& id, const QPixmap& icon)
{
    if (auto* slot = firstFreeSlot()) {
        slot->setItem(icon, id);
        return true;
    }
    return false;
}

void InventoryWidget::toggleVisible()
{
    setVisible(!isVisible());
    if (isVisible()) raise();
}

/* al terminar el drag comprobamos si cayó sobre la papelera */
void InventoryWidget::handleBeginDrag(ItemSlot* src)
{
    connect(qApp, &QApplication::focusChanged,
            src, [this, src](QWidget*, QWidget*)
            {
                QWidget* tgt = QApplication::widgetAt(QCursor::pos());
                if (tgt == m_trash) src->clearSlot();
                disconnect(qApp, nullptr, src, nullptr);
            });
}
