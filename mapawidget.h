#ifndef MAPAWIDGET_H
#define MAPAWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include "sprite.h"
#include "grafomapa.h"

class MapaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapaWidget(const QString& regionActual, QWidget *parent = nullptr);
    ~MapaWidget();  // ✅ Declarar el destructor

    void setRegionActual(const QString &region);  // ✅ Declarar esta función

signals:
    void mapaCerrado();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onFrame();

private:
    QPixmap m_imagenMapa;
    Sprite *m_jugadorSprite;
    QTimer *m_timer;
    QString m_regionActual;
    GrafoMapa m_grafo;
};

#endif // MAPAWIDGET_H
