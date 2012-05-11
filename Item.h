
#pragma once

#include "Object.h"

#include <string>
#include <vector>
#include <map>

/* An Item is a tree where each node is made of something and has a shape. A
 * root node, however, has no shape or material. It acts only as a placeholder
 * for its components.
 */

struct Material
{
    std::string adjective;

    int density;
    int durrability;
};

extern std::map< std::string, Material > materials;

struct Shape
{
    int volume;
    char image;
};

extern std::map< std::string, Shape > shapes;

bool operator == ( const Shape&, const Shape& );

extern Material air, wood, hair, skin, glass, healingPotion;
extern Shape nothing, rod, wig, hand, broom, bottle, liquid;

struct Item : public Object
{
    std::vector< Item > components;

    std::string material;
    std::string shape;

    Item();

    // A leaf item.
    Item( const std::string& mat, const std::string& shp );
    // A container. (image = !) 
    Item( const std::string& mat, const std::string& shp, const Item& );

    // A complex item.
    Item( const std::string& shape, const Item& mainCmp, const Item& second );
};

typedef std::map< const char* const, Item > Catalogue;
extern Catalogue catalogue;

void init_items();

int mass( const Item& );
int durrability( const Item& );

typedef std::vector< Item > Inventory;
