
#pragma once

#include "Object.h"

#include <string>
#include <vector>

/* An Item is a tree where each node is made of something and has a shape. A
 * root node, however, has no shape or material. It acts only as a placeholder
 * for its components.
 */

struct Material
{
    std::string name;
    std::string adjective;

    int density;
};

struct Shape
{
    std::string name;

    int volume;
};

bool operator == ( const Shape&, const Shape& );

extern Material air, wood, hair, skin;
extern Shape nothing, rod, wig, hand, broom;

struct Item : public Object
{
    std::vector< Item > components;

    const Material * material;
    const Shape    * shape;

    Item();
    Item( char image, const Material*, const Shape* );
    Item( const Shape*, const Item& mainCmp, const Item& second );
};

int mass( const Item& );

typedef std::vector< Item > Inventory;
