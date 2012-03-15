
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

void print_map()
{
    // Instead of painting the map, then items, then actors onto the  screen,
    // just copy the map to a buffer, paint everything to it,  and paint it to
    // the screen. This means that buffer can be put  anywhere on screen
    // without making sure everything's being  painted with the same offset.
    Map toScreen = map;
    FOR_EACH( items, Item&  i, toScreen[i.pos.y][i.pos.x] = i.image );
    FOR_EACH( actors,  Actor& n, toScreen[n.pos.y][n.pos.x] = n.image );

    Actor& player = actors.front();
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
