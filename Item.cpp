
#include "Item.h"

#include <algorithm>
#include <climits>

//       mat   :   name    adjective  dens  dur
Material air   = { "air",   "air",       0,  -1 };
Material wood  = { "wood",  "wooden",   10,  50 };
Material skin  = { "skin",  "skin",      5, 200 };
Material glass = { "glass", "glass",     4,  10 };
Material hair  = { "horse hair", "horse hair", 2, 0 };
Material healingPotion = { "health", "healing", 1, 0 };

Shape nothing = { "nothing", 0 };
Shape rod     = { "rod",     5 };
Shape wig     = { "wig",     1 };
Shape hand    = { "hand",    3 }; 
Shape broom   = { "broom",   0 };
Shape bottle  = { "bottle",  5 };
Shape liquid  = { "potion",  5 };

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
    name = m->adjective + " " + s->name;
}

// A root item is nothing; just a placeholder for its parts.
Item::Item( const Shape* s, const Item& main, const Item& scnd )
    : Object(main.pos, main.image), 
      material(&air), shape(s)
{
    name = main.material->adjective + " " + s->name;
    components.push_back( main );
    components.push_back( scnd );
}

Item::Item( const Material* m, const Shape* s, const Item& i )
    : Object({0,0}, '!'), components(1,i), material(m), shape(s)
{
    name = s->name + " of " + i.material->name;
}

template< typename F >
int _accumulate_item( const Item& i, F f )
{
    int sum = f( i );
    for( const auto& c : i.components )
        sum += _accumulate_item( c, f );
    return sum;
}

int mass( const Item& i )
{
    auto m = []( const Item& i ){ 
        return i.material->density * i.shape->volume; 
    };
    return _accumulate_item( i, m );
}

int durrability( const Item& i )
{
    auto d = [](const Item& i){ 
        return i.material->durrability; 
    };
    return _accumulate_item( i, d );
}
