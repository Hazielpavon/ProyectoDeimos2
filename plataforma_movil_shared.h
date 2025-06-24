#pragma once

struct MovingPlatform {
    QGraphicsPixmapItem* sprite;
    QGraphicsRectItem*   hitbox;
    float                 minX, maxX;
    float                 speed;
    int                   dir;      // +1 = derecha, -1 = izquierda
};
