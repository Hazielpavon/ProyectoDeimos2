// GrafoMapa.h
#ifndef GRAFOMAPA_H
#define GRAFOMAPA_H

#pragma once
#include <QString>
#include <QPoint>
#include <QMap>
#include <QVector>
#include <QPair>
#include <optional>

class GrafoMapa {
public:
    void agregarRegion(const QString &nombre, const QPoint &posicion);
    void conectar(const QString &origen, const QString &destino, int peso);
    void cargarRegiones();
    void conectarRegiones();
    QList<QString> rutaMasCorta(const QString &inicio, const QString &fin);
    QPoint posicionRegion(const QString &region) const;
    std::optional<QPoint> posicion(const QString &region) const;

private:
    QMap<QString, QVector<QPair<QString, int>>> m_grafo;
    QMap<QString, QPoint> m_posiciones;
};

#endif // GRAFOMAPA_H
