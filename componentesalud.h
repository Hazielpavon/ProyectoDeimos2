#ifndef COMPONENTESALUD_H
#define COMPONENTESALUD_H

#include <QPainter>
#include <QPoint>

class componentesalud
{
public:
    componentesalud();

    void setHP(int hp);
    int  currentHP() const;
    int  maxHP() const;
    void aplicarDano(int dmg);
    void actualizar(float dt);
    //Muestra La salud del personaje principal y a futuro el de los enemigos
    void dibujar(QPainter &painter, const QPoint &posSprite);

private:
    int m_currentHP;
    int m_maxHP;
};
#endif // COMPONENTESALUD_H
