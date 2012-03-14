
#pragma once

#include "Vec.h"

#include <string>

struct Item
{
    enum Material
    {
        WOOD,
        HAIR,
        SKIN
    };

    enum Type
    {
        ROD,
        WIG,
        HAND
    };

    Vec pos;
    std::string name;
    char     image;
    Material material;
    Type     type;

    Item( Vec pos, const std::string& name, char image, 
          Material material, Type type );
};

