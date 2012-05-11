
#include "Item.h"

#include <algorithm>
#include <climits>

std::map< std::string, Shape > shapes;
std::map< std::string, Material > materials;

char _img( const std::string& shape )
{
    return shapes[ shape ].image;
}

Item::Item()
    : Object({0,0}, ' ')
{
}

// Leaf item.
Item::Item( const std::string& m, const std::string& s )
    : Object({0,0}, _img(s)), material(m), shape(s)
{
    name = materials[m].adjective + " " + s;
}

// A root item is nothing; just a placeholder for its parts.
Item::Item( const std::string& s, const Item& main, const Item& scnd )
    : Object(main.pos, main.image), 
      material("air"), shape(s)
{
    name = materials[main.material].adjective + " " + s;
    components.push_back( main );
    components.push_back( scnd );
}

Item::Item( const std::string& m, const std::string& s, const Item& i )
    : Object({0,0}, '!'), components(1,i), material(m), shape(s)
{
    name = m + " " + s + " of " + i.material;
}

void init_items()
{
    materials["air"] = { "air", 0, INT_MAX };
    materials["wood"] = { "wooden", 10, 50 };
    materials["skin"] = { "skinny", 5, 200 };
    materials["glass"] = { "glassy", 4, 10 };
    materials["horse"] = { "horse",  2,  0 };
    materials["health"] = { "healing", 1, 0 };

    shapes["nothing"] = { 0, ' ' };
    shapes["rod"]     = { 5, '/' };
    shapes["wig"]     = { 1, '\'' };
    shapes["hand"]    = { 3, 'F' };
    shapes["broom"]   = { 0, '/' };
    shapes["bottle"]  = { 5, '!' };
    shapes["potion"]  = { 5, 'o' };
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
        return materials[i.material].density * shapes[i.shape].volume; 
    };
    return _accumulate_item( i, m );
}

int durrability( const Item& i )
{
    auto d = [](const Item& i){ 
        return materials[i.material].durrability; 
    };
    return _accumulate_item( i, d );
}
