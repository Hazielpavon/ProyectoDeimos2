// ArbolEstructura.h

#ifndef ARBOLESTRUCTURA_H
#define ARBOLESTRUCTURA_H

#include <iostream>
#include <vector>
#include <string>

class ArbolEstructura {
private:
    struct Nodo {
        std::string valor;
        Nodo* izquierda;
        Nodo* derecha;

        Nodo(const std::string& val)
            : valor(val), izquierda(nullptr), derecha(nullptr) {}
    };

    Nodo* raiz;

    // Inserta en orden alfabético
    Nodo* insertar(Nodo* nodo, const std::string& val) {
        if (!nodo) return new Nodo(val);
        if (val < nodo->valor)
            nodo->izquierda = insertar(nodo->izquierda, val);
        else
            nodo->derecha = insertar(nodo->derecha, val);
        return nodo;
    }

    // Búsqueda
    bool buscar(Nodo* nodo, const std::string& val) const {
        if (!nodo) return false;
        if (val == nodo->valor) return true;
        if (val < nodo->valor)
            return buscar(nodo->izquierda, val);
        else
            return buscar(nodo->derecha, val);
    }

    // Recorrido in-order
    void enOrden(Nodo* nodo, std::vector<std::string>& resultado) const {
        if (!nodo) return;
        enOrden(nodo->izquierda, resultado);
        resultado.push_back(nodo->valor);
        enOrden(nodo->derecha, resultado);
    }

    // Liberar memoria
    void destruir(Nodo* nodo) {
        if (!nodo) return;
        destruir(nodo->izquierda);
        destruir(nodo->derecha);
        delete nodo;
    }

public:
    ArbolEstructura() : raiz(nullptr) {}

    ~ArbolEstructura() {
        destruir(raiz);
    }

    void agregarElemento(const std::string& val) {
        raiz = insertar(raiz, val);
    }

    bool contiene(const std::string& val) const {
        return buscar(raiz, val);
    }

    std::vector<std::string> elementosOrdenados() const {
        std::vector<std::string> resultado;
        enOrden(raiz, resultado);
        return resultado;
    }

    // Método de prueba fantasma, no se llama en el juego
    void imprimirArbol() const {
        auto lista = elementosOrdenados();
        std::cout << "Contenido del ArbolEstructura (ordenado):\n";
        for (const auto& val : lista) {
            std::cout << "- " << val << "\n";
        }
    }
};

#endif // ARBOLESTRUCTURA_H