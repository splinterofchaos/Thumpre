
#pragma once

#include "Object.h"

#include <string>
#include <vector>

struct Item : public Object
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

    std::string name;
    Material material;
    Type     type;

    Item( Vec pos, const std::string& name, char image, 
          Material material, Type type );
};

typedef std::vector< Item > Inventory;
