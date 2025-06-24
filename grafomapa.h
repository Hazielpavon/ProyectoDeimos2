#ifndef GRAFOMAPA_H
#define GRAFOMAPA_H

#include <QString>
#include <QMap>
#include <QVector>
#include <QPoint>
#include <QPair>
#include <QList>
#include <optional>

class GrafoMapa
{
public:
    void agregarRegion(const QString &nombre, const QPoint &posicion);
    void conectar(const QString &origen, const QString &destino, int peso = 1);

    void cargarRegiones();
    void conectarRegiones();

    QList<QString> rutaMasCorta(const QString &inicio, const QString &fin);
    QPoint posicionRegion(const QString &region) const;
    std::optional<QPoint> posicion(const QString &region) const;

    QList<QPair<QString, QString>> conexionesVisibles() const;
    QList<QString> vecinosDe(const QString &region) const;
    QVector<QString> conexionesDesde(const QString &region) const;

    QMap<QPair<QString, QString>, QVector<QPoint>> obtenerRutasManuales() const;
    void agregarRutaManual(const QString &origen,
                           const QString &destino,
                           const QVector<QPoint> &puntos);
    QVector<QPoint> rutaManual(const QString &origen,
                               const QString &destino) const;
    const QMap<QString, QVector<QPair<QString,int>>>& grafo() const
    {
        return m_grafo;
    }
    QList<QList<QString>> todasLasRutas(const QString &origen, const QString &destino) const;
    double obtenerDistanciaVisual(const QString &origen, const QString &destino) const;
    QMap<QPair<QString, QString>, double> m_distanciasVisibles;


private:
    QMap<QString, QVector<QPair<QString, int>>> m_grafo;
    QMap<QString, QPoint>                       m_posiciones;
    QMap<QPair<QString, QString>, QVector<QPoint>> m_rutasManuales;
};

#endif // GRAFOMAPA_H
