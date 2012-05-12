
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
    std::vector< std::string > possibleMakup;
    int volume;
    char image;
};

extern std::map< std::string, Shape > shapes;

bool operator == ( const Shape&, const Shape& );

extern Material air, wood, hair, skin, glass, healingPotion;
extern Shape nothing, rod, wig, hand, broom, bottle, liquid;

typedef std::vector< std::string > Inventory;
struct Item : public Object
{
    Inventory components;

    std::string material;
    std::string shape;

    Item();

    Item( const std::string& mat, const std::string& shp );
};

Item basic_item( const std::string& material, const std::string& shape );
Item container_item( const std::string& material, const std::string& shape, 
                     const std::string& contents );
Item complex_item( const std::string& shape, 
                   const std::string& mainCmp, const std::string& scndCmp );

typedef std::map< std::string, Item > Catalogue;
extern Catalogue catalogue;

const std::string& add_to_catalogue( const Item& i );

void init_items();

int mass( const std::string& );
int durrability( const std::string& );

