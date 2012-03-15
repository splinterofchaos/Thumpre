/* Map Gen, the generic (roguelike) map generator. */

#include <stdio.h>
#include <stdlib.h>

#include <time.h> // For time() to generate random seed.

typedef const char* const literal;

struct Vec { int x; int y; };

int random( int till, int from=0 )
{
    return rand() % (till-from) + from;
}

int main( int argc, char** argv )
{
    literal usage = "mapgen ([X]x[Y])\n"
                    "   Where X and Y represent the size of the output map.";

    if( argc < 2 )
    {
        printf( "Two arguments required.\n %s", usage );
        exit( 1 );
    }

    int width, height;
    sscanf( argv[1], "%dx%d", &width, &height );

    char** map = new char*[ height ];
    for( int i=0; i < height; i++ )
    {
        map[i] = new char[ width + 1];
        map[i][width] = '\0';
    }

    for( int y=0; y < height; y++ )
    {
        for( int x=0; x < width; x++ )
            map[y][x] = '#';
    }

    // For now, just create a room at a random position.
    srand(time(0));
    int startX = random( width/2,  1 );
    int startY = random( height/2, 1 );
    int endX = random( width -1,  startX + 3 );
    int endY = random( height-1, startY + 3 );

    for( int y=startY; y <= endY; y++ )
        for( int x=startX; x <= endX; x++ )
            map[y][x] = '.';


    for( int y=0; y < height; y++ )
        printf( "%s\n", map[y] );

    for( int y=0; y < height; y++ )
        delete [] map[y];
    delete [] map;
}
