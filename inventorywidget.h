#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

/* ───────────── ItemSlot ───────────── */
class ItemSlot : public QLabel
{
    Q_OBJECT
public:
    explicit ItemSlot(QWidget* parent=nullptr);

    bool isEmpty() const      { return m_empty; }
    void setItem(const QPixmap& pix, const QString& id);
    void clearSlot();

signals:
    void beginDrag(ItemSlot* src);

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseDoubleClickEvent(QMouseEvent*) override;

private:
    bool    m_empty = true;
    QString m_id;          // “LlaveRoja”, etc.
};

/* ───────────── InventoryWidget ───────────── */
class InventoryWidget : public QWidget
{
    Q_OBJECT
public:
    static InventoryWidget* instance(QWidget* parent = nullptr);

    bool addKey(const QString& id, const QPixmap& icon);

public slots:
    void toggleVisible();

private slots:
    void handleBeginDrag(ItemSlot* src);

private:
    InventoryWidget(QWidget* parent=nullptr);
    ItemSlot* firstFreeSlot() const;

    static InventoryWidget* s_inst;

    QGridLayout* m_grid  = nullptr;
    ItemSlot*    m_trash = nullptr;   // “basurero”
};
