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
    auto clave = qMakePair(origen, destino);
    if (!m_rutasManuales.contains(clave)) {
        qWarning() << "No se conectaron" << origen << "↔" << destino << "porque no hay ruta visual";
        return;
    }

    if (m_grafo.contains(origen) && m_grafo.contains(destino)) {
        m_grafo[origen].append(qMakePair(destino, peso));
        m_grafo[destino].append(qMakePair(origen, peso));
    }
}

void GrafoMapa::cargarRegiones()
{
    agregarRegion("Templo del Silencio", QPoint(130, 260));
    agregarRegion("Raices Olvidadas", QPoint(130, 400));
    agregarRegion("Ciudad Inversa", QPoint(280, 260));
    agregarRegion("Torre de la Marca", QPoint(350, 380));
    agregarRegion("Máquina del Olvido", QPoint(520, 288));
    agregarRegion("Mente Vacía", QPoint(520, 402));

    agregarRutaManual("Templo del Silencio", "Ciudad Inversa", {
                                                                   QPoint(150, 260), QPoint(180, 270), QPoint(190, 270), QPoint(200, 275), QPoint(210, 280), QPoint(230, 280), QPoint(250, 290),
                                                                   QPoint(270, 295), QPoint(285, 300), QPoint(300, 260)
                                                               });

    agregarRutaManual("Templo del Silencio", "Raices Olvidadas", {
                                                                     QPoint(150, 260), QPoint(180, 270), QPoint(190, 270), QPoint(200, 275), QPoint(210, 280), QPoint(230, 280), QPoint(250, 290),
                                                                     QPoint(270, 295), QPoint(285, 300), QPoint(285, 305), QPoint(280, 310), QPoint(275, 315), QPoint(270, 320), QPoint(250, 340),
                                                                     QPoint(225, 355), QPoint(210, 365), QPoint(195, 370), QPoint(190, 380), QPoint(180, 390), QPoint(170, 390), QPoint(150, 400)
                                                                 });

    agregarRutaManual("Raices Olvidadas", "Torre de la Marca", {
                                                                   QPoint(150, 400), QPoint(170, 390), QPoint(180, 390), QPoint(190, 380), QPoint(195, 370), QPoint(210, 365), QPoint(225, 355),
                                                                   QPoint(235, 355), QPoint(245, 358), QPoint(255, 360), QPoint(270, 370), QPoint(290, 385), QPoint(305, 395), QPoint(320, 400),
                                                                   QPoint(340, 395), QPoint(350, 385), QPoint(360, 380)
                                                               });

    agregarRutaManual("Ciudad Inversa", "Máquina del Olvido", {
                                                                  QPoint(300, 260), QPoint(310, 260), QPoint(320, 255), QPoint(340, 250), QPoint(350, 250), QPoint(370, 258), QPoint(400, 274),
                                                                  QPoint(420, 288), QPoint(440, 305), QPoint(460, 315), QPoint(470, 320), QPoint(490, 290), QPoint(500, 280), QPoint(510, 280), QPoint(530, 288)
                                                              });

    agregarRutaManual("Máquina del Olvido", "Mente Vacía", {
        QPoint(530, 288), QPoint(510, 280), QPoint(500, 280), QPoint(490, 290), QPoint(470, 320), QPoint(490, 330), QPoint(505, 335), QPoint(510, 340)
    });

    agregarRutaManual("Torre de la Marca", "Mente Vacía", {
                                                              QPoint(380, 380), QPoint(390, 380), QPoint(400, 390), QPoint(410, 395), QPoint(430, 410), QPoint(450, 420), QPoint(470, 420),
                                                              QPoint(490, 420), QPoint(505, 410), QPoint(520, 400)
                                                          });
}

void GrafoMapa::conectarRegiones()
{
    conectar("Templo del Silencio", "Ciudad Inversa", 2);
    conectar("Templo del Silencio", "Raices Olvidadas", 2);
    conectar("Raices Olvidadas", "Torre de la Marca", 2);
    conectar("Ciudad Inversa", "Máquina del Olvido", 2);
    conectar("Máquina del Olvido", "Mente Vacía", 2);
    conectar("Torre de la Marca", "Mente Vacía", 2);
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

            // 🔒 Solo permitimos conexiones que tengan ruta visual (manual)
            auto clave = qMakePair(actual, nombreVecino);
            if (!m_rutasManuales.contains(clave)) continue;

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

QList<QList<QString>> GrafoMapa::todasLasRutas(const QString &origen, const QString &destino) const {
    QList<QList<QString>> resultados;
    QList<QString> rutaActual;
    QSet<QString> visitados;

    std::function<void(QString)> dfs = [&](QString nodo) {
        rutaActual.append(nodo);

        if (nodo == destino) {
            resultados.append(rutaActual);
        } else {
            visitados.insert(nodo);
            for (const auto &vecino : m_grafo.value(nodo)) {
                QString vecinoNombre = vecino.first;
                auto clave = qMakePair(nodo, vecinoNombre);

                // Validaciones estrictas:
                bool noVisitado = !visitados.contains(vecinoNombre);
                bool tieneRutaVisual = m_rutasManuales.contains(clave);
                bool noRepetido = !rutaActual.contains(vecinoNombre);

                if (noVisitado && tieneRutaVisual && noRepetido) {
                    dfs(vecinoNombre);
                } else if (!tieneRutaVisual) {
                    qWarning() << "❌ Saltando conexión sin ruta visual:" << nodo << "→" << vecinoNombre;
                } else if (!noRepetido) {
                    qWarning() << "🔁 Saltando ciclo repetido:" << vecinoNombre << "ya en" << rutaActual;
                }
            }
            visitados.remove(nodo);
        }

        rutaActual.removeLast();
    };

    dfs(origen);

    // 🔍 Debug final: imprime todas las rutas generadas
    qDebug() << "📌 Rutas generadas desde" << origen << "a" << destino << ":";
    for (const QList<QString> &ruta : resultados)
        qDebug() << "   · " << ruta;

    return resultados;
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

QList<QPair<QString, QString>> GrafoMapa::conexionesVisibles() const {
    QList<QPair<QString, QString>> conexiones;

    for (auto it = m_grafo.begin(); it != m_grafo.end(); ++it) {
        const QString &origen = it.key();
        for (const auto &dest : it.value()) {
            const QString &destino = dest.first;
            if (origen < destino) {
                conexiones.append(qMakePair(origen, destino));
            }
        }
    }

    return conexiones;
}

QList<QString> GrafoMapa::vecinosDe(const QString &region) const {
    QList<QString> vecinos;
    if (m_grafo.contains(region)) {
        for (const auto &par : m_grafo[region]) {
            vecinos.append(par.first);
        }
    }
    return vecinos;
}

QVector<QString> GrafoMapa::conexionesDesde(const QString &region) const {
    QVector<QString> conexiones;
    if (m_grafo.contains(region)) {
        for (const auto &par : m_grafo[region]) {
            conexiones.append(par.first);
        }
    }
    return conexiones;
}

void GrafoMapa::agregarRutaManual(const QString &origen, const QString &destino, const QVector<QPoint> &puntos) {
    m_rutasManuales[qMakePair(origen, destino)] = puntos;
    m_rutasManuales[qMakePair(destino, origen)] = puntos;

    // Calcular distancia visual de la ruta
    double distancia = 0.0;
    for (int i = 0; i < puntos.size() - 1; ++i)
        distancia += std::hypot(puntos[i+1].x() - puntos[i].x(), puntos[i+1].y() - puntos[i].y());

    double km = distancia * 0.01;

    m_distanciasVisibles[qMakePair(origen, destino)] = km;
    m_distanciasVisibles[qMakePair(destino, origen)] = km;
}
QVector<QPoint> GrafoMapa::rutaManual(const QString &origen, const QString &destino) const
{
    auto clave = qMakePair(origen, destino);
    if (m_rutasManuales.contains(clave)) {
        return m_rutasManuales.value(clave);
    }
    return {};
}

double GrafoMapa::obtenerDistanciaVisual(const QString &origen, const QString &destino) const {
    return m_distanciasVisibles.value(qMakePair(origen, destino), 0.0);
}
QMap<QPair<QString, QString>, QVector<QPoint>> GrafoMapa::obtenerRutasManuales() const {
    return m_rutasManuales;
}

