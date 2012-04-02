
#include "Tiles.h"

#include <algorithm>

Tiles::Tiles()
{
    tiles = new char[1];
    flipped = false;
    width = height = 1;
}

Tiles::Tiles( unsigned int width, unsigned int height )
    : width(width), height(height)
{
    tiles = new char[ width * height ];
    flipped = false;
}

Tiles::~Tiles()
{
    delete [] tiles;
}

void Tiles::flip()
{
    flipped = not flipped;
    std::swap( width, height );
}

void Tiles::reset( unsigned int w, unsigned int h )
{
    width = w; height = h;
    flipped = false;

    if( tiles )
        delete [] tiles;
    tiles = new char[ width * height ];

    std::fill_n( tiles, width*height, '#' );
}

char& Tiles::get( unsigned int x, unsigned int y )
{
    return const_cast<char&>(
        static_cast<const Tiles*>(this)->get(x,y)
    );
}

const char& Tiles::get( unsigned int x, unsigned int y ) const
{
    if( flipped )
        return tiles[ y*width + x ];
    else
        return tiles[ x*height + y ];
}
