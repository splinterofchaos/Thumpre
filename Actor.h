
#pragma once

#include "Vec.h"
#include "Item.h"

struct Actor
{
    Vec pos;
    Inventory inventory;

    int hp;
    int speed;

    int cooldown; // Time until next move.

    char image;

    bool playerControlled;

    Actor( Vec pos, char image );
};
