/* Map Gen, the generic (roguelike) map generator. */

#include <stdio.h>
#include <stdlib.h>

#include <time.h> // For time() to generate random seed.

#include <string>
#include <vector>
#include <algorithm>

#include "Tiles.h"
#include "Vec.h"

typedef const char* const literal;
typedef unsigned int uint;

struct Range 
{ 
    uint min, max; 

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
    Area( uint xMin, uint xMax, uint yMin, uint yMax );

    uint operator () ();

    Vec center();
};

Area::Area( const Range& h, const Range& v )
    : horizontal(h), vertical(v)
{
}

Area::Area( uint xMin, uint xMax, uint yMin, uint yMax )
    : horizontal(xMin,xMax), vertical(yMin,yMax)
{
}

uint Area::operator() ()
{
    return horizontal.size() * vertical.size();
}

Vec Area::center()
{
    return Vec (
        (horizontal.max + horizontal.min) / 2,
        (vertical  .max + vertical  .min) / 2
    );
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
    if( till - from <= 0 )
        return 0;
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
    do
    {
        Area a (
            random_range( area.horizontal, size ),
            random_range( area.vertical,   size )
        );

        float hratio = std::abs( a.horizontal.size() / a.vertical.size() );
        const float MAX = 2, IMAX = 1/MAX;
        if( a() >= size.min and a() <= size.max 
            and hratio < MAX and hratio > IMAX )
            return a;
    } while( true );
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

void dig_hallway( Vec a, Vec b, Tiles& m )
{
    auto sgn = []( int x ){ return x>0? 1 : x<0? -1 : 0; };

    Vec step( sgn(b.x-a.x), sgn(b.y-a.y) );
    for( int x = a.x; x != b.x; x += step.x )
        m.get( x, a.y ) = '.';
    for( int y = a.y; y != b.y; y += step.y )
        m.get( b.x, y ) = '.';
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

    // Keep track of the center of each room we create.
    std::vector< Vec > centers;
    centers.reserve( nRooms );

    uint n = nRooms;
    while( n-- )
    {
        Area bounds( {1, m.width-2}, {1, m.height-2} );
        Range size( 3, AREA / (nRooms*2) );
        Area area = random_area( bounds, size );
        
        dig_room( area, m );

        // Dig hallways between rooms so that each room can (more likely) be accessed.
        centers.push_back( area.center() );
        if( centers.size() > 2 )
            dig_hallway ( 
                centers.back(), 
                centers[ random(centers.size() - 2) ],
                m
            );
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

    for( uint y=0; y < map.height; y++ )
    {
        for( uint x=0; x < map.width; x++ )
            printf( "%c", map.get(x, y) );
        printf( "\n" );
    }
}
