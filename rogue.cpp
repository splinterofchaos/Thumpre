
#include "World.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime> // To produce seed for srand.

#include <vector>
#include <list>
#include <string>
#include <algorithm>

#include <ncurses.h>

// This just makes using std::for_each a little easier to write/read.
#define RNG( container ) container.begin(), container.end()
#define FOR_EACH( container, value, block ) \
    std::for_each( RNG(container), [&]( const value ){ block; } )

Vec mapPos, inventoryPos, logPos;

// Produces a random, non-wall, non-occupied position.
// It asserts that a free space exists and never exits otherwise.
Vec random_position()
{
    while( true )
    {
        Vec p( 0, 0 );
        p.y = std::rand() % map.size();
        p.x = std::rand() % map[0].size();

        if( item_at(p)  == items.end()  and 
            actor_at(p) == actors.end()  and
            map[p.y][p.x] != '#' )
            return p;
    }
}

bool read_map()
{
    FILE* mapgen = popen( "./mapgen 20x15", "r" ); 
    if( not mapgen )
        return false;

    char row[ 200 ];
    while( fscanf(mapgen, "%s", row) != EOF )
        map.push_back( row );
    pclose( mapgen );

    return true;
}

void show_bresenham_line( Map* dst, Vec from, Vec to, const Map& src )
{
    bool steep = std::abs( to.y - from.y ) > std::abs( to.x - from.x );

    if( steep )
    {
        std::swap( to.x, to.y );
        std::swap( from.x, from.y );
    }

    Vec delta = to - from; 
    delta.x = std::abs( delta.x );
    delta.y = std::abs( delta.y );
    
    int error = delta.x / 2;

    Vec step( 0, 0 );

    if( to.x > from.x )
        step.x = 1;
    else if( to.x < from.x )
        step.x = -1;

    if( to.y > from.y )
        step.y = 1;
    else if( to.y < from.y )
        step.y = -1;

    int y = from.y;
    for( int x = from.x; x != to.x; x += step.x )
    {
        if( x < 0 or x >= map[0].size() or
            y < 0 or y >= map.size() )
            return;

        char& cDst = steep ? (*dst)[x][y] : (*dst)[y][x];
        const char& cSrc = steep ? src[x][y] : src[y][x];
        cDst = cSrc;

        if( cDst == '#' )
            return;

        error -= delta.y;
        if( error < 0 )
        {
            y += step.y;
            error += delta.x;
        }
    }
}

void print_map()
{
    Actor& player = actors.front();

    // Instead of painting the map, then items, then actors onto the  screen,
    // just copy the map to a buffer, paint everything to it,  and paint it to
    // the screen. This means that buffer can be put  anywhere on screen
    // without making sure everything's being  painted with the same offset.
    Map toScreen( map.size() );
    std::string line( map[0].size(), ' ' );
    std::fill( toScreen.begin(), toScreen.end(), line );

    static Vec visualLimits[] = { Vec(5,0), Vec(4,1), Vec(4,2), Vec(3,3),
        Vec(2,4), Vec(1,4), Vec(0,5), Vec(-1,4), Vec(-2,4), Vec(-3,3),
        Vec(-4,2), Vec(-4,1), Vec(-5,0), Vec(-4,-1), Vec(-4,-2), Vec(-3,-3),
        Vec(-2,-4), Vec(-1,-4), Vec(0,-5), Vec(1,-4), Vec(2,-4), Vec(3,-3),
        Vec(4,-2), Vec(4,-1) };

    for( int i = 0; i < sizeof(visualLimits)/sizeof(visualLimits[0]); i++ )
        show_bresenham_line( &toScreen, player.pos, player.pos + visualLimits[i], map );

    FOR_EACH( items, Item&  i, toScreen[i.pos.y][i.pos.x] = i.image );
    FOR_EACH( actors,  Actor& n, toScreen[n.pos.y][n.pos.x] = n.image );


    toScreen[player.pos.y][player.pos.x] = player.image;

    unsigned int row = 0;
    FOR_EACH ( 
        toScreen, std::string& line, 
        mvprintw( mapPos.y + row++, mapPos.x, line.c_str() )
    );
}

void print_log()
{
    unsigned int row = 0;
    FOR_EACH ( 
        logger, std::string& msg, 
        mvprintw( logPos.y + row++, logPos.x, msg.c_str() ) 
    );
    logger.clear();
}

void print_inventory()
{
    // Print the inventory.
    mvprintw( inventoryPos.y++, inventoryPos.x, "You have:" );

    unsigned int y = 0;
    unsigned int x = inventoryPos.x + 2;
    if( actors[0].inventory.size() )
        FOR_EACH ( 
            actors.front().inventory, Item& i, 
            mvprintw( inventoryPos.y + y++, x, "%c - %s", 'a'+y, i.name.c_str() ) 
        );
    else
        mvprintw( inventoryPos.y + y, inventoryPos.x + 2, "Nothing." );
}

int main( int argc, char** argv )
{
    initscr();
    cbreak();
    noecho();
    refresh();
    keypad(stdscr, TRUE);

    std::srand( std::time(0) );

    read_map();

    actors.push_back( Actor(random_position(), '@') );
    actors.back().playerControlled = true;

    actors.push_back( Actor(random_position(), 'k') );

    items.push_back( Item(random_position(), "Broom Handle", '/', Item::WOOD, Item::ROD) );
    items.push_back( Item(random_position(), "Horse Hair",   '"', Item::HAIR, Item::WIG) );


    while( not quit )
    {
        const int MAP_TOP = 5;
        const int MAP_BOTTOM = MAP_TOP + map.size();

        logPos = Vec( 2, 1 );
        inventoryPos = Vec( 4, MAP_BOTTOM + 2 );
        mapPos = Vec( 2, MAP_TOP );

        erase();
        
        Inventory::iterator itemHere = item_at( actors[0].pos );

        if( itemHere != items.end() )
            logger.push_back( "Your foot hits a " + itemHere->name + "." );

        print_map();
        print_inventory();
        print_log();

        refresh(); 

        move_player( &actors[0] );
    }


    endwin();

    return 0;
}
