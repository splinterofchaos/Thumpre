
#include "Actor.h"

Actor::Actor( Vec pos, char image )
: pos(pos), image(image)
{
    hp = 100;
    speed = 5;
    strength = 5;

    cooldown = 0;
    playerControlled = false;

    //equipment[ EQ_MAIN_WEAPON ] = unarmed_weapon();
}

Item Actor::unarmed_weapon() const
{
    return Item( {0,0}, "Fist", 'F', Item::SKIN, Item::HAND );
}
