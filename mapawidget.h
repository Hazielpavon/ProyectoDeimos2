#ifndef MAPAWIDGET_H
#define MAPAWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QList>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include "sprite.h"
#include "grafomapa.h"

class MapaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapaWidget(const QString& regionActual, QWidget *parent = nullptr);
    ~MapaWidget();

    void setRegionActual(const QString &region);
    void setRutaActual(const QList<QString> &ruta);

signals:
    void mapaCerrado();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onFrame();
    void animarRuta();  // ⚠️ Debe declararse aquí para que no dé error en el cpp

private:
    QPixmap m_imagenMapa;
    Sprite *m_jugadorSprite;
    QTimer *m_timer;
    QString m_regionActual;
    GrafoMapa m_grafo;

    QList<QString> m_rutaActual;
    QMap<QString, QLabel*> m_clickLabels;
    QPushButton *m_botonBorrarRuta;

    // 🔴 Variables para animación visual de la ruta
    QTimer *m_animacionTimer;
    int m_animacionIndex;
    QList<QPoint> m_puntosRuta;         // Puntos de la ruta completa
    QList<QPoint> m_puntosAnimados;     // Puntos que ya se han "dibujado"
};

#endif // MAPAWIDGET_H
