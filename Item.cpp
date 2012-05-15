
#include "Item.h"
#include "Log.h"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstring> // for strcpy
#include <cctype> // for isspace

std::map< std::string, Shape > shapes;
std::map< std::string, Material > materials;
Catalogue catalogue;
Inventory possibleItems;

Item::Item()
    : Object({0,0}, ' ')
{
}

char _img( const std::string& shape )
{
    return shapes[ shape ].image;
}

// Leaf item.
Item::Item( const std::string& m, const std::string& s )
    : Object({0,0}, _img(s)), material(m), shape(s)
{
}

Item basic_item( const std::string& m, const std::string& s )
{
    Item i( m, s );

    // Example name: wooded box.
    i.name = materials[m].adjective + " " + s;

    return i;
}

Item container_item( const std::string& m, const std::string& s, const std::string& c )
{
    Item i = basic_item( m, s );

    // Example: wooden box of rocks.
    i.name += " of " + c;
    i.components.push_back( c );

    return i;
}

Item complex_item( const std::string& shape, 
                   const std::string& mainCmp, const std::string& scndCmp )
{
    const auto& ref = catalogue[ mainCmp ];

    // A complex object takes after its main component.
    Item i = basic_item( ref.material, shape );
    i.components.push_back( mainCmp );
    i.components.push_back( scndCmp );
    return i;
}

const std::string& add_to_catalogue( const Item& i )
{
    const auto& entry = catalogue.find( i.name );
    if( entry == catalogue.end() )
        catalogue[ i.name ] = i;
    return i.name;
}

void init_items()
{
    // TODO: It'd be nice if this were done in some script.

    materials["wood" ] = { "wooden", 10,      50 };
    materials["iron" ] = { "iron",   20, INT_MAX };
    materials["steel"] = { "steel",  30, INT_MAX };
    materials["skin" ] = { "skin",    5,     200 };
    materials["glass"] = { "glass",  10,      10 };
    materials["horse"] = { "horse",   4, INT_MAX };
    materials["health"] = { "healing", 1, INT_MAX };
    
    shapes["rod" ] = { 5, '/' };
    shapes["wig" ] = { 2, '"' };
    shapes["hand"] = { 3, ' ' };
    shapes["bottle"] = { 1, '!' };
    shapes["potion"] = { 2, 'O' };

    add_to_catalogue( basic_item("wood", "rod") );
    add_to_catalogue( basic_item("iron", "rod") );
    add_to_catalogue( basic_item("steel", "rod") );
    add_to_catalogue( basic_item("health", "potion") );
    add_to_catalogue( basic_item("glass", "bottle") );
    add_to_catalogue( container_item("glass", "bottle", "healing potion") );
    add_to_catalogue( basic_item("horse", "wig") );
    add_to_catalogue( basic_item("hand", "fist") );
    log( "%s", basic_item("wood", "rod").name.c_str() );
}

template< typename F >
int _accumulate_item( const std::string& name, F f )
{
    auto iter = catalogue.find( name );
    if( iter == catalogue.end() )
    {
        log( "_accumulated_item: %s does not exist.", name.c_str() );
        return 0;
    }

    auto& i = iter->second;
    int sum = f( i );
    for( const auto& c : i.components )
        sum += _accumulate_item( c, f );
    return sum;
}

int mass( const std::string& name )
{
    auto m = []( const Item& i ){ 
        return materials[i.material].density * shapes[i.shape].volume; 
    };
    return _accumulate_item( name, m );
}

int durrability( const std::string& name )
{
    auto d = [](const Item& i){ 
        return materials[i.material].durrability; 
    };
    return _accumulate_item( name, d );
}

