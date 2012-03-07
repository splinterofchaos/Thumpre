
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <vector>
#include <string>

#include <ncurses.h>

typedef std::vector< std::string > Map;
Map map;

bool equal( const char* const a, const char* const b )
{
    return strcmp( a, b ) == 0;
}

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
        WOOD
    };

    enum Type
    {
        ROD
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

struct Actor
{
    typedef std::vector< Item > Inventory;
    typedef Inventory::iterator InvIt;

    Vec pos;
    Inventory inventory;

    Actor( int x, int y )
        : pos( x, y )
    {
    }

    Actor( Vec pos )
        : pos( pos )
    {
    }
};

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
        FILE* mapgen = popen( "./mapgen 20x20", "r" ); 
        if( not mapgen )
            return 1;

        char row[ 200 ];
        while( fscanf(mapgen, "%s", row) != EOF )
            map.push_back( row );
        pclose( mapgen );
    }

    Actor player( 0, 0 );

    // (0,0) is an illegal position. If the player has an x, y is implied as
    // both are set at once. Loop until x (and implicitly, y) is set.
    for( int y=1; not player.pos.x and y<map.size(); y++ )
        for( int x=1; not player.pos.x and x<map[y].size(); x++ )
            if( map[y][x] != '#' )
                player.pos = { x, y };

    Item broomHandle( player.pos, "Broom Handle", '/', Item::WOOD, Item::ROD );

    if( not player.pos.x )
        return 2;

    Vec messagePos;
    messagePos.x = 3;
    messagePos.y = map.size() + 3;

    bool quit = false;
    while( not quit )
    {
        static std::string lastMessage = "Hello";

        erase();
        
        if( player.pos == broomHandle.pos )
        {
            const std::string what = "Your foot hits a " + broomHandle.name + ".";
            if( lastMessage != "" )
                lastMessage = lastMessage + "; " + what;
            else
                lastMessage = what;
        }

        for( unsigned int row=0; row < map.size(); row++ )
            mvprintw( row, 0, map[row].c_str() );
        mvaddch( broomHandle.pos.y, broomHandle.pos.x, broomHandle.image );
        mvaddch( player.pos.y, player.pos.x, '@' );
        mvprintw( messagePos.y, messagePos.x, lastMessage.c_str() );

        // Print the inventory.
        mvprintw( messagePos.y + 3, 4, "You have:" );
        if( player.inventory.size() )
            for( int i = 0; i < player.inventory.size(); i++ )
                mvprintw( messagePos.y + 4+i, 6, player.inventory[i].name.c_str() );
        else
            mvprintw( messagePos.y + 4, 6, "Nothing." );

        refresh(); 

        lastMessage = "";

        Vec inputDir = { 0, 0 }; // The direction the user wants to move, if any.

        int key = getch();
        switch( key )
        {
          case 'l': case KEY_RIGHT: case '6': inputDir = Vec(  1,  0 ); break;
          case 'h': case KEY_LEFT:  case '4': inputDir = Vec( -1,  0 ); break;
          case 'k': case KEY_UP:    case '8': inputDir = Vec(  0, -1 ); break;
          case 'j': case KEY_DOWN:  case '2': inputDir = Vec(  0,  1 ); break;
          case 'y': case '7': inputDir = Vec( -1, -1 ); break;
          case 'u': case '9': inputDir = Vec(  1, -1 ); break;
          case 'b': case '1': inputDir = Vec( -1,  1 ); break;
          case 'n': case '3': inputDir = Vec(  1,  1 ); break;

          case 'p': case 'g': if( broomHandle.pos == player.pos )
                              {
                                  player.inventory.push_back( broomHandle );

                                  // Just take yourself off the map.
                                  // TODO: There may be... better, much better solutions.
                                  broomHandle.pos = Vec( -1, -1 );
                              }

          case 'c': lastMessage = "..."; break;
          case 'q': quit = true; break;
          default: lastMessage = "Is that key supposed to do something?"; break;
        }

        if( inputDir.x or inputDir.y )
            if( not walk(&player, inputDir) )
                lastMessage = "Cannot move there.";
    }


    endwin();

    return 0;
}
