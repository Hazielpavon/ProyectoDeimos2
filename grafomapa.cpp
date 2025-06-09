#include "grafomapa.h"
#include <limits>
#include <queue>
#include <QDebug>

void GrafoMapa::agregarRegion(const QString &nombre, const QPoint &posicion)
{
    if (!m_grafo.contains(nombre)) {
        m_grafo[nombre] = QVector<QPair<QString, int>>();
        m_posiciones[nombre] = posicion;
    }
}

void GrafoMapa::conectar(const QString &origen, const QString &destino, int peso)
{
    if (m_grafo.contains(origen) && m_grafo.contains(destino)) {
        m_grafo[origen].append(qMakePair(destino, peso));
        m_grafo[destino].append(qMakePair(origen, peso)); // bidireccional
    }
}

void GrafoMapa::cargarRegiones()
{
    agregarRegion("Templo del Silencio", QPoint(225, 260));
    agregarRegion("Raises Olvidadas", QPoint(225, 400));
    agregarRegion("Ciudad Inversa", QPoint(380, 260));
    agregarRegion("Torre de la Marca", QPoint(450, 380));
    agregarRegion("Máquina del Olvido", QPoint(613, 288));
    agregarRegion("Mente Vacía", QPoint(618, 402));

}

void GrafoMapa::conectarRegiones()
{
    conectar("Templo del Silencio", "Torre de la Marca", 2);
    conectar("Torre de la Marca", "Máquina del Olvido", 3);
    conectar("Máquina del Olvido", "Pozo de los Caídos", 4);
    conectar("Pozo de los Caídos", "Raíces Olvidadas", 2);
    conectar("Raíces Olvidadas", "Ciudad Inversa", 5);
}

QList<QString> GrafoMapa::rutaMasCorta(const QString &inicio, const QString &fin)
{
    QMap<QString, int> distancias;
    QMap<QString, QString> predecesores;
    QSet<QString> visitados;

    for (const QString &region : m_grafo.keys()) {
        distancias[region] = std::numeric_limits<int>::max();
    }
    distancias[inicio] = 0;

    auto cmp = [&distancias](const QString &a, const QString &b) {
        return distancias[a] > distancias[b];
    };

    std::priority_queue<QString, QVector<QString>, decltype(cmp)> cola(cmp);
    cola.push(inicio);

    while (!cola.empty()) {
        QString actual = cola.top();
        cola.pop();

        if (visitados.contains(actual)) continue;
        visitados.insert(actual);

        for (const auto &vecino : m_grafo[actual]) {
            QString nombreVecino = vecino.first;
            int peso = vecino.second;

            if (distancias[actual] + peso < distancias[nombreVecino]) {
                distancias[nombreVecino] = distancias[actual] + peso;
                predecesores[nombreVecino] = actual;
                cola.push(nombreVecino);
            }
        }
    }

    QList<QString> ruta;
    QString actual = fin;
    while (actual != inicio && predecesores.contains(actual)) {
        ruta.prepend(actual);
        actual = predecesores[actual];
    }
    if (actual == inicio)
        ruta.prepend(inicio);

    return ruta;
}

QPoint GrafoMapa::posicionRegion(const QString &region) const
{
    return m_posiciones.value(region, QPoint(0, 0));
}

std::optional<QPoint> GrafoMapa::posicion(const QString &region) const
{
    if (m_posiciones.contains(region)) {
        return m_posiciones[region];
    }
    return std::nullopt;
}
