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
    void setRutaActual(const QString &destino); // ← para calcular todas las rutas posibles al destino


signals:
    void mapaCerrado();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onFrame();
    void animarRuta();

private:
    QPixmap m_imagenMapa;
    QPixmap m_imagenInformacion;
    Sprite *m_jugadorSprite;
    QTimer *m_timer;
    QTimer *m_animacionTimer;

    QString m_regionActual;
    GrafoMapa m_grafo;

    QList<QString> m_rutaActual;
    QList<QPoint> m_puntosRuta;
    QList<QPoint> m_puntosAnimados;
    int m_animacionIndex = 0;

    QLabel *m_rutaInfoLabel = nullptr;
    QPushButton *m_botonBorrarRuta;
    QMap<QString, QLabel*> m_clickLabels;

    QVector<QPoint> m_rutaMasCortaPuntos;
    QVector<QVector<QPoint>> m_rutasAlternativasPuntos;

    // 🔹 NUEVO PARA DIBUJAR TODAS LAS RUTAS:
    bool mostrarTodasRutas = false;
    QVector<QList<QString>> m_rutasParaDibujar;

    void crearClickableLabel(const QString &region);

};

#endif // MAPAWIDGET_H
