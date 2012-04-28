
#include "Map.h"

#include <algorithm>

Map::Map()
{
    flipped = false;
    dims = Vec( 0, 0 );
}

Map::Map( Vec dims )
    : tiles( dims.y, Row(dims.x, ' ') )
{
}

Map::~Map()
{
}

void Map::flip()
{
    flipped = not flipped;
    std::swap( dims.x, dims.y );
}

void Map::clear()
{
    tiles.clear();
}

void Map::add_row( Row r )
{
    if( not dims.x )
        dims.x = r.size();
    dims.y++;

    tiles.push_back( r );
}

Map::Row& Map::row( unsigned int r )
{
    return tiles[r];
}

char& Map::get( const Vec& v )
{
    return const_cast<char&>(
        static_cast<const Map*>(this)->get(v)
    );
}

const char& Map::get( const Vec& v ) const
{
    if( flipped )
        return tiles[ v.x ][ v.y ];
    else
        return tiles[ v.y ][ v.x ];
}
