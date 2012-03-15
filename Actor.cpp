
#include "Actor.h"

Actor::Actor( Vec pos, char image )
: pos( pos ), image(image)
{
    hp = 100;
    playerControlled = false;
}
