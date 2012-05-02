
#pragma once

#include "Object.h"
#include "Item.h"

struct Actor : public Object
{
    Inventory inventory;

    int hp;
    int speed;

    int cooldown; // Time until next move.

    bool playerControlled;

    Actor( Vec pos, char image );
};
