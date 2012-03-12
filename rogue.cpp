
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <vector>
#include <string>
#include <algorithm>

#include <ncurses.h>

struct Vec
{
    int x, y;

    Vec() {}
    Vec( int x, int y ) : x(x), y(y) {}
};

bool operator == ( const Vec& a, const Vec& b )
{
    return a.x == b.x and a.y == b.y;
}

Vec operator + ( const Vec& a, const Vec& b )
{
    return { a.x + b.x, a.y + b.y };
}

struct Item
{
    enum Material
    {
        WOOD,
        HAIR
    };

    enum Type
    {
        ROD,
        WIG
    };

    Vec pos;
    std::string name;
    char     image;
    Material material;
    Type     type;

    Item( Vec pos, const std::string& name, char image, Material material, Type type )
        : pos(pos), name(name), image(image), material(material), type(type)
    {
    }
};

typedef std::vector< std::string > Map;
typedef std::vector< Item > Inventory;
Map map;
Inventory items;
std::string lastMessage = "Hello";
bool quit = false;

struct Actor
{
    Vec pos;
    Inventory inventory;

    char image;

    Actor( Vec pos, char image )
        : pos( pos ), image(image)
    {
    }

    virtual void move() = 0;
};

// TODO: When the code gets better organized, 
// these oddly placed prototypes won't be necessary.
bool walk( Actor* a, Vec dir );
Inventory::iterator item_at( Vec pos );
void transfer( Inventory* to, Inventory* from, Inventory::iterator what );

struct Player : public Actor
{
    Player( Vec pos )
        : Actor( pos, '@' )
    {
    }

    void move()
    {
        // The direction the user wants to move, if any.
        Vec inputDir = { 0, 0 }; 

        // We need to know if we're standing on an item, but can't construct
        // the object inside the switch, so do it now.
        Inventory::iterator itemHere = item_at( pos );

        int key = getch();
        switch( key )
        {
          case 'l': case '6': case KEY_RIGHT: inputDir = Vec(  1,  0 ); break;
          case 'h': case '4': case KEY_LEFT:  inputDir = Vec( -1,  0 ); break;
          case 'k': case '8': case KEY_UP:    inputDir = Vec(  0, -1 ); break;
          case 'j': case '2': case KEY_DOWN:  inputDir = Vec(  0,  1 ); break;
          case 'y': case '7': inputDir = Vec( -1, -1 ); break;
          case 'u': case '9': inputDir = Vec(  1, -1 ); break;
          case 'b': case '1': inputDir = Vec( -1,  1 ); break;
          case 'n': case '3': inputDir = Vec(  1,  1 ); break;

          case 'p': case 'g': 
            if( itemHere != items.end() )
            {
                transfer( &inventory, &items, itemHere );
                lastMessage = "Got " + inventory.back().name + ".";
            }
            else
                lastMessage = "There's nothing here.";

            break;

          case 'c': lastMessage = "..."; break;
          case 'q': quit = true; break;
          default: lastMessage = "Is that key supposed to do something?"; break;
        }

        if( inputDir.x or inputDir.y )
            if( not walk(this, inputDir) )
                lastMessage = "Cannot move there.";
    }
};

struct Npc : public Actor
{
    Npc( Vec pos, char image )
        : Actor( pos, image )
    {
    }

    void move()
    {
        // TODO
    }
};

typedef std::vector< Npc > NpcList;
NpcList npcs;

void transfer( Inventory* to, Inventory* from, Inventory::iterator what )
{
    to->push_back( *what );
    from->erase( what );
}

Inventory::iterator item_at( Vec pos )
{
    return std::find_if ( 
        items.begin(), items.end(), [&](const Item& i) { return i.pos == pos; }
    );
}

bool walk( Actor* a, Vec dir )
{
    Vec newPos = a->pos + dir;

    bool inBounds = newPos.x > 0 and newPos.y > 0;
    inBounds &= newPos.y < map.size() and newPos.x < map[newPos.y].size();

    if( inBounds and map[newPos.y][newPos.x] != '#' )
    {
        a->pos = newPos;
        return true;
    }

    return false;
}

int main( int argc, char** argv )
{
    initscr();
    //cbreak();
    noecho();
    refresh();
    keypad(stdscr, TRUE);

    { // Read in the map from mapgen, the generic map generator.
        FILE* mapgen = popen( "./mapgen 20x15", "r" ); 
        if( not mapgen )
            return 1;

        char row[ 200 ];
        while( fscanf(mapgen, "%s", row) != EOF )
            map.push_back( row );
        pclose( mapgen );
    }

    Player player( Vec(0,0) );

    // (0,0) is an illegal position. If the player has an x, y is implied as
    // both are set at once. Loop until x (and implicitly, y) is set.
    for( int y=1; not player.pos.x and y<map.size(); y++ )
        for( int x=1; not player.pos.x and x<map[y].size(); x++ )
            if( map[y][x] != '#' )
                player.pos = { x, y };

    npcs.push_back( Npc(player.pos+Vec(0,3), 'k') );

    items.push_back( Item(player.pos, "Broom Handle", '/', Item::WOOD, Item::ROD) );
    items.push_back( Item(player.pos+Vec(1,0), "Horse Hair", '"', Item::HAIR, Item::WIG) );

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
        {
            const std::string what = "Your foot hits a " + itemHere->name + ".";
            if( lastMessage != "" )
                lastMessage = lastMessage + "; " + what;
            else
                lastMessage = what;
        }

        #define RNG( container ) container.begin(), container.end()
        #define FOR_EACH( container, value, block ) \
            std::for_each( RNG(container), [&]( const value ){ block; } )

        unsigned int row = 0;
        FOR_EACH( map,    std::string& line, mvprintw(row++, 0, line.c_str()) );
        FOR_EACH( items,  Item& i,  mvaddch(i.pos.y, i.pos.x, i.image ) );
        FOR_EACH( npcs, Npc& n, mvaddch(n.pos.y, n.pos.x, n.image) );

        mvaddch( player.pos.y, player.pos.x, player.image );

        mvprintw( messagePos.y, messagePos.x, lastMessage.c_str() );
        lastMessage = "";

        // Print the inventory.
        mvprintw( messagePos.y + 3, 4, "You have:" );
        if( player.inventory.size() )
        {
            unsigned int y = 0;
            FOR_EACH ( 
                player.inventory, Item& i, 
                mvprintw( messagePos.y + 4 + y++, 6, i.name.c_str() ) 
            );
        }
        else
            mvprintw( messagePos.y + 4, 6, "Nothing." );

        refresh(); 

        player.move();
    }


    endwin();

    return 0;
}
