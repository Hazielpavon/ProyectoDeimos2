#ifndef JUGADOR_H
#define JUGADOR_H

#include <vector>
#include <any>
#include "entidad.h"

class Jugador
{
public:
    Jugador();

    entidad&       getVida()       { return vida; }
    const entidad& getVida() const { return vida; }

    std::vector<std::any>&       getObjetos()       { return objetos; }
    const std::vector<std::any>& getObjetos() const { return objetos; }

    std::vector<std::any>&       getLlaves()       { return llaves; }
    const std::vector<std::any>& getLlaves() const { return llaves; }

    // métodos genéricos para agregar
    template<typename T>
    void agregarObjeto(const T& obj) { objetos.push_back(obj); }

    template<typename T>
    void agregarLlave(const T& llave) { llaves.push_back(llave); }


private:
    entidad               vida;
    std::vector<std::any> objetos;
    std::vector<std::any> llaves;
};

#endif // JUGADOR_H
