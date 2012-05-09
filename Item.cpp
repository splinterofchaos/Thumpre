
#include "Item.h"

#include <algorithm>

Material air  = { "air",  "air",     0 };
Material wood = { "wood", "wooden", 10 };
Material skin = { "skin", "skin",    5 };
Material hair = { "horse hair", "horse hair", 2 };

Shape nothing = { "nothing", 0 };
Shape rod     = { "rod",  5 };
Shape wig     = { "wig",  1 };
Shape hand    = { "hand", 3 }; 
Shape broom   = { "broom", 0 };

bool operator == ( const Shape& a, const Shape& b )
{
    return a.name == b.name and a.volume == b.volume;
}

Item::Item()
    : Object({0,0}, ' ')
{
}

// Leaf item.
Item::Item( char image, const Material* m, const Shape* s )
    : Object({0,0}, image), material(m), shape(s)
{
    this->name = m->adjective + " " + s->name;
}

// A root item is nothing; just a placeholder for its parts.
Item::Item( const Shape* s, const Item& main, const Item& scnd )
    : Object(main.pos, main.image), 
      material(&air), shape(s)
{
    this->name = main.material->adjective + " " + s->name;
    components.push_back( main );
    components.push_back( scnd );
}


int mass( const Item& i )
{
    int sum = i.material->density * i.shape->volume;
    for( const auto& c : i.components )
        sum += mass( c );
    return sum;
}
