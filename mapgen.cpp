/* Map Gen, the generic (roguelike) map generator. */

#include <stdio.h>
#include <stdlib.h>

#include <time.h> // For time() to generate random seed.

#include <string>
#include <vector>
#include <algorithm>

#include "Tiles.h"

typedef const char* const literal;
typedef unsigned int uint;

struct Vec { int x; int y; };
struct Rect { int left; int right; int up; int down; };

struct Range 
{ 
    int min, max; 

    Range( int m, int M );
    
    uint size();
};

Range::Range( int m, int M )
    : min(m), max(M)
{
}

uint Range::size()
{
    return max - min;
}

struct Area 
{ 
    Range horizontal, vertical;
    Area( const Range& h, const Range& v );
    Area( int xMin, int xMax, int yMin, int yMax );

    int operator () ();
};

Area::Area( const Range& h, const Range& v )
    : horizontal(h), vertical(v)
{
}

Area::Area( int xMin, int xMax, int yMin, int yMax )
    : horizontal(xMin,xMax), vertical(yMin,yMax)
{
}

int Area::operator() ()
{
    return horizontal.size() * vertical.size();
}

Tiles map;

Range normalize( Range r )
{
    if( r.min > r.max )
        std::swap( r.min, r.max );
    return r;
}

int random( int till, int from=0 )
{
    return rand() % (till-from) + from;
}

Range random_range( const Range& rng, const Range& size )
{
    auto r = [&](){ return random(rng.max+1, rng.min); };
    Range rr /*random range*/ = normalize({ r(), r() });
    
    if( rr.size() < size.min or rr.size() > size.max )
        return random_range( rng, size );
    else
        return rr;
}

Area random_area( Area area, Range size )
{
    Area a (
        random_range( area.horizontal, size ),
        random_range( area.vertical,   size )
    );

    if( a() < size.min or a() > size.max )
        return random_area( area, size );
    else
        return a;
}

void make_map( char* argDimensions )
{
    uint w, h;
    sscanf( argDimensions, "%dx%d", &w, &h );

    map.reset( w, h );
}

int difference( int a, int b )
{
    return std::abs( b - a );
}

void dig_room( const Area& a, Tiles& m )
{
    for( uint x = a.horizontal.min; x < a.horizontal.max; x++ )
        for( uint y = a.vertical.min; y<a.vertical.max; y++ )
            m.get(x,y) = '.';
}

void dig_splatter_pattern( const uint nRooms, Tiles& m )
{
    // The area we have to work with in m.
    const int AREA = (m.width-2) * (m.height-2);

    uint n = nRooms;
    while( n-- )
    {
        Area bounds( {1, m.width-2}, {1, m.height-2} );
        Range size( 3, AREA / nRooms );
        dig_room( random_area(bounds, size), m );
    }
}

void dig( int n, Tiles& m )
{
    dig_splatter_pattern( n, m );
}

int main( int argc, char** argv )
{
    literal usage = "mapgen [X]x[Y]\n"
                    "   Where X and Y represent the size of the output map.";

    if( argc < 2 )
    {
        printf( "Two arguments required.\n %s", usage );
        exit( 1 );
    }

    make_map( argv[1] );

    // For now, just create a room at a random position.
    srand(time(0));

    dig( 10, map );

    for( int y=0; y < map.height; y++ )
    {
        for( int x=0; x < map.width; x++ )
            printf( "%c", map.get(x, y) );
        printf( "\n" );
    }
}
