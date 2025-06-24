#pragma once

#include <qgraphicsitem.h>
struct MovingPlatform {
    QGraphicsPixmapItem* sprite;
    QGraphicsRectItem*   hitbox;
    float                 minX, maxX;
    float                 speed;
    int                   dir;
};
