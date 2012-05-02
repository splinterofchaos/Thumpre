
#include "World.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime> // To produce seed for srand.
#include <cmath> 

#include <vector>
#include <list>
#include <string>
#include <algorithm>

#include <ncurses.h>
#include <signal.h>

// After initscr, if the OS sends us a segfault or termination signal, we need
// to call endwin to avoid screwing up the user's terminal. 
void end_window_and_terminate( int sig, siginfo_t* info, void* arg )
{
    endwin();
    perror( "Signal to terminate recieved" );
    exit(1);
}

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
        p.x = std::rand() % map.dims.x;
        p.y = std::rand() % map.dims.y;

        if( actor_at(p) == actors.end() 
            and map.get (p) != '#' )
            return p;
    }
}

bool read_map()
{
    FILE* mapgen = popen( "./mapgen 25x15", "r" ); 
    if( not mapgen )
        return false;

    char row[ 200 ];
    while( fscanf(mapgen, "%s", row) != EOF )
        map.add_row( row );
    pclose( mapgen );

    return true;
}

int sgn( int x )
{
    return x > 0 ? 1 : x < 0 ? -1 : 0;
}

bool has_adjacent_foor_tile( const Vec& place, const Map& src )
{
    bool safe = false;

    Vec pos (
        std::max( 0, place.x - 1 ),
        std::max( 0, place.y - 1 )
    );

    Vec end (
        std::min( (int)map.dims.y, place.x + 2 ),
        std::min( (int)map.dims.x, place.y + 2 )
    );

    Vec offset( 0, 0 );

    for( ; not safe and offset.x+pos.x != end.x; offset.x++ ) 
        for( ; offset.y+pos.y != end.y; offset.y++ )
            if( not (offset.x == 1 and offset.y == offset.x) 
                and src.get(pos+offset) == '.' )
            {
                safe = true;
                break;
            }

    return safe;
}

void show_bresenham_line( Vec start, Vec v )
{
    bool steep = std::abs( v.y ) > std::abs( v.x );

    struct {
        void operator () ( Vec& a ) { std::swap( a.x, a.y ); }
    } flip;

    if( steep )
    {
        flip( v );
        flip( start );
    }
    
    // Specify which direction to move.
    Vec step( sgn, v );
    // The absolute difference between the start and end of a ray.
    Vec delta( std::abs<int>, v );

    int error = delta.x / 2;

    int y = start.y;
    for( int x = start.x; x != start.x + v.x; x += step.x )
    {
        Vec mapDims( map.dims );
        if( steep ) flip( mapDims );

        if( x < 0 or x >= mapDims.x or
            y < 0 or y >= mapDims.y )
            return;

        Vec mapPlace( x, y );
        if( steep )
            flip( mapPlace );

        map.visible( mapPlace, true );
        if( map.get(mapPlace) == '#' )
            return;

        error -= delta.y;
        if( error < 0 )
        {
            y += step.y;
            error += delta.x;
        }
    }
}

void show_quadrant( const Vec& quad, const Vec& pos )
{
    for( int x = 0; x != quad.x; x += sgn(quad.x) )
        show_bresenham_line( pos, Vec(x, quad.y) );
    for( int y = 0; y != quad.y; y += sgn(quad.y) )
        show_bresenham_line( pos, Vec(quad.x, y) );

    show_bresenham_line( pos, quad );
}

void print( Vec p, char c )
{
    p = p + mapPos;
    mvaddch( p.y, p.x, c );
}

void print_object( const Object& object )
{
    if( map.visible(object.pos) )
        print( object.pos, object.image );
}

void print_map()
{
    for( Vec p(0,0); p.y < map.dims.y; p.y++ )
        for( p.x = 0 ; p.x < map.dims.x; p.x++ )
            map.visible( p, false );

    int r = 5;
    Vec corners[] = { Vec(r,r), Vec(-r,r), Vec(-r,-r), Vec(r,-r) };
    for( uint i = 0; i < 4; i++ )
        show_quadrant( corners[i], actors.front().pos );

    for( Vec p(0,0); p.y < map.dims.y; p.y++ )
        for( p.x = 0 ; p.x < map.dims.x; p.x++ )
        {
            auto tile = map.tile( p );
            if( tile.visible )
                print( p, tile.c );
        }

    std::for_each( items.begin(),  items .end(), print_object );
    std::for_each( actors.begin(), actors.end(), print_object );
}

void print_log()
{
    uint row = 0;
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

    uint x = inventoryPos.x + 2;
    if( actors[0].inventory.size() )
    {
        uint y = 0;
        FOR_EACH ( 
            actors.front().inventory, Item& i, 
            mvprintw( inventoryPos.y + y++, x, "%c - %s", 'a'+y, i.name.c_str() ) 
        );
    }
    else
    {
        mvprintw( inventoryPos.y, x, "Nothing." );
    }
}

int main()
{
    initscr();
    cbreak();
    noecho();
    refresh();
    keypad(stdscr, TRUE);

    std::srand( std::time(0) );
    int* i;

    {
        struct sigaction sact;
        memset( &sact, 0, sizeof sact );
        sact.sa_sigaction = end_window_and_terminate;
        sact.sa_flags     = SA_SIGINFO;
        sigaction( SIGSEGV, &sact, 0 );
        sigaction( SIGKILL, &sact, 0 );
        sigaction( SIGINT, &sact, 0 );
    }

    read_map();

    actors.push_back( Actor(random_position(), '@') );
    actors.back().playerControlled = true;

    actors.push_back( Actor(random_position(), 'k') );

    items.push_back( Item(random_position(), "Broom Handle", '/', Item::WOOD, Item::ROD) );
    items.push_back( Item(random_position(), "Horse Hair",   '"', Item::HAIR, Item::WIG) );

    unsigned int time = 0;

    while( not quit )
    {
        const int MAP_TOP = 5;
        const int MAP_BOTTOM = MAP_TOP + map.dims.x;

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

        auto quickest = std::min_element (
            actors.begin(), actors.end(),
            []( const Actor& a, const Actor& b ) {
                return a.cooldown < b.cooldown;
            }
        );

        if( quickest->playerControlled )
        {
            move_player( *quickest );
            quickest->cooldown += 1;
        }
        else
        {
            walk( *quickest, Vec(1,0) );
            quickest->cooldown += 1;
        } 
    }

    endwin();

    return 0;
}
