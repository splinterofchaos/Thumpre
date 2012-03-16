/* Map Gen, the generic (roguelike) map generator. */

#include <stdio.h>
#include <stdlib.h>

#include <time.h> // For time() to generate random seed.

#include <string>
#include <vector>
#include <algorithm>

typedef const char* const literal;

struct Vec { int x; int y; };
struct Rect { int left; int right; int up; int down; };

typedef std::vector< std::string > Map;
Map map;
int width;
int height;

int random( int till, int from=0 )
{
    return rand() % (till-from) + from;
}

void make_map( char* argDimensions )
{
    sscanf( argDimensions, "%dx%d", &width, &height );

    map.reserve( height );

    int i = height;
    while( i-- )
        map.push_back( std::string(width, '#') );
}

bool add_room( const Rect& area )
{
    // Make sure no room exists in this area.
    for( int y=area.up; y<area.down; y++ )
        for( int x=area.left; x<area.right; x++ )
            if( map[y][x] == '.' )
                return false;

    for( int y=area.up; y<area.down; y++ )
        for( int x=area.left; x<area.right; x++ )
            map[y][x] = '.';

    return true;
}

Rect random_rect( int minW, int minH )
{
    Rect ret;
    
    ret.left = random( width  - minW - 1, 1 );
    ret.up   = random( height - minH - 1, 1 );

    ret.right = random( width  - 1, ret.left + minW );
    ret.down  = random( height - 1, ret.up   + minH);

    if( ret.left > ret.right or ret.up > ret.down )
        printf( "ERROR" );

    return ret;
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

    add_room( random_rect(3,3) );

    for( int y=0; y < height; y++ )
        printf( "%s\n", map[y].c_str() );
}
