
#pragma once

#include "Vec.h"

#include <string>

struct Object
{
    Vec pos;
    char image;
    std::string name;

    Object( Vec pos, char image );
};
