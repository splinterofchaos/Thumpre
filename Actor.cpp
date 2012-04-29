
#include "Actor.h"

Actor::Actor( Vec pos, char image )
: pos(pos), image(image)
{
    hp = 100;
    speed = 5;
    cooldown = 0;
    playerControlled = false;
}
