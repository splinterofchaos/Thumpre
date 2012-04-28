
#include "Map.h"

#include <algorithm>

Map::Map()
{
    flipped = false;
    dims = Vec( 0, 0 );
}

Map::Map( Vec dims )
    : tiles( dims.y, Row(dims.x, {' ', false}) )
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

void Map::add_row( const std::string& rowstr )
{
    if( not dims.x )
        dims.x = rowstr.size();
    dims.y++;

    Row row; row.reserve( dims.x );
    std::for_each ( 
        rowstr.begin(), rowstr.end(), 
        [&](const char c){ row.push_back({c,false}); }
    );

    tiles.push_back( row );
}

std::string Map::row( unsigned int r ) const
{
    std::string s; s.reserve( dims.x );
    auto row = tiles[r];
    std::for_each (
        row.begin(), row.end(),
        [&](const Tile& t){ s.push_back(t.c); }
    );

    return s;
}

Map::Tile& Map::tile( const Vec& v )
{
    return const_cast<Tile&>(
        static_cast<const Map*>(this)->tile(v)
    );
}

const Map::Tile& Map::tile( const Vec& v ) const
{
    if( flipped )
        return tiles[ v.x ][ v.y ];
    else
        return tiles[ v.y ][ v.x ];
}

bool Map::visible( const Vec& v ) const
{
    return tile( v ).visible;
}

void Map::visible( const Vec& v, bool canSeeIt )
{
    tile( v ).visible = canSeeIt;
}

char& Map::get( const Vec& v )
{
    return tile( v ).c;
}

const char& Map::get( const Vec& v ) const
{
    return tile( v ).c;
}
