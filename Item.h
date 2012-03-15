
#pragma once

#include "Vec.h"

#include <string>
#include <vector>

struct Item
{
    enum Material
    {
        WOOD,
        HAIR
    };

    enum Type
    {
        ROD,
        WIG
    };

    Vec pos;
    std::string name;
    char     image;
    Material material;
    Type     type;

    Item( Vec pos, const std::string& name, char image, 
          Material material, Type type );
};

typedef std::vector< Item > Inventory;