
#pragma once

#include "Object.h"
#include "Item.h"

struct Actor : public Object
{
    // Backpack.
    Inventory inventory;

    // Weapons, armor, etc.
    enum { EQ_MAIN_WEAPON, N_SLOTS };
    //Item equipment[ N_SLOTS ];

    int hp;
    int speed;
    int strength;

    float cooldown; // Time until next move.

    bool playerControlled;

    Actor( Vec pos, char image );

    Item unarmed_weapon() const;
};
