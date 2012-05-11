
#include "Actor.h"

Actor::Actor( Vec pos, char image )
    : Object(pos,image)
{
    if( image == '@' )
        name = "_player";
    else
        name = "_moster";
    name[0] = image;

    hp = 100;
    speed = 5;
    strength = 5;

    cooldown = 0;
    playerControlled = false;

    //equipment[ EQ_MAIN_WEAPON ] = unarmed_weapon();
}

Item Actor::unarmed_weapon() const
{
    return Item( "skin", "hand" );
}
