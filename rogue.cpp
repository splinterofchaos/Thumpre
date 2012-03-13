
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

// Produces a random, non-wall, non-occupied position.
// It asserts that a free space exists and never exits otherwise.
Vec random_position()
{
    while( true )
    {
        Vec p( 0, 0 );
        p.y = std::rand() % map.size();
        p.x = std::rand() % map[0].size();

        if( item_at(p) == items.end() and 
            npc_at(p)  == npcs.end()  and
            p != player.pos           and
            map[p.y][p.x] != '#' )
            return p;
    }
}

int main( int argc, char** argv )
{
    initscr();
    cbreak();
    noecho();
    refresh();
    keypad(stdscr, TRUE);

    std::srand( std::time(0) );

    { // Read in the map from mapgen, the generic map generator.
        FILE* mapgen = popen( "./mapgen 20x15", "r" ); 
        if( not mapgen )
            return 1;

        char row[ 200 ];
        while( fscanf(mapgen, "%s", row) != EOF )
            map.push_back( row );
        pclose( mapgen );
    }

    player.pos = random_position();

    npcs.push_back( Npc(random_position(), 'k') );

    items.push_back( Item(random_position(), "Broom Handle", '/', Item::WOOD, Item::ROD) );
    items.push_back( Item(random_position(), "Horse Hair",   '"', Item::HAIR, Item::WIG) );

    if( not player.pos.x )
        return 2;

    Vec messagePos;
    messagePos.x = 3;
    messagePos.y = map.size() + 3;

    while( not quit )
    {
        erase();
        
        Inventory::iterator itemHere = item_at( player.pos );

        if( itemHere != items.end() )
            logger.push_back( "Your foot hits a " + itemHere->name + "." );

        #define RNG( container ) container.begin(), container.end()
        #define FOR_EACH( container, value, block ) \
            std::for_each( RNG(container), [&]( const value ){ block; } )

        const int MAP_TOP = 5;
        const int MAP_BOTTOM = MAP_TOP + map.size();

        { // Draw the map.
            // Instead of painting the map, then items, then actors onto the
            // screen, just copy the map to a buffer, paint everything to it,
            // and paint it to the screen. This means that buffer can be put
            // anywhere on screen without making sure everything's being
            // painted with the same offset.
            Map toScreen = map;
            FOR_EACH( items, Item& i, toScreen[i.pos.y][i.pos.x] = i.image );
            FOR_EACH( npcs,  Npc&  n, toScreen[n.pos.y][n.pos.x] = n.image );

            toScreen[player.pos.y][player.pos.x] = player.image;

            unsigned int row = 0;
            FOR_EACH ( 
                toScreen, std::string& line, 
                mvprintw(MAP_TOP + row++, 2, line.c_str())
            );
        }

        unsigned int row = 0;
        FOR_EACH( logger, std::string& msg, mvprintw(1 + row++, 1, msg.c_str()) );
        logger.clear();

        // Print the inventory.
        row = MAP_BOTTOM + 2;
        mvprintw( row++, 4, "You have:" );
        if( player.inventory.size() )
        {
            FOR_EACH ( 
                player.inventory, Item& i, 
                mvprintw( row++, 6, i.name.c_str() ) 
            );
        }
        else
            mvprintw( row, 6, "Nothing." );

        refresh(); 

        player.move();
    }


    endwin();

    return 0;
}
