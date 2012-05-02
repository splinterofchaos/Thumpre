
#pragma once

#include "Vec.h"
#include "Item.h"

struct Actor
{
    Vec pos;

    // Backpack.
    Inventory inventory;

    // Weapons, armor, etc.
    enum { EQ_MAIN_WEAPON, N_SLOTS };
    //Item equipment[ N_SLOTS ];

    int hp;
    int speed;
    int strength;

    float cooldown; // Time until next move.

    char image;

    bool playerControlled;

    Actor( Vec pos, char image );

    Item unarmed_weapon() const;
};
